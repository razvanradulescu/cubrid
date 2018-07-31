/*
 * Copyright (C) 2008 Search Solution Corporation. All rights reserved by Search Solution.
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

/*
 * log_generator.cpp
 */

#ident "$Id$"

#include "log_generator.hpp"
#include "replication_stream_entry.hpp"
#include "multi_thread_stream.hpp"
#include "log_impl.h"
#include "heap_file.h"
#include "system_parameter.h"
#include "error_manager.h"

namespace cubreplication
{
  bool enable_log_generator_logging = false;

  log_generator::~log_generator ()
  {
    m_stream_entry.destroy_objects ();
  }

  int log_generator::start_tran_repl (MVCCID mvccid)
  {
    m_stream_entry.set_mvccid (mvccid);

    return NO_ERROR;
  }

  int log_generator::set_commit_repl (bool commit_tran_flag)
  {
    m_stream_entry.set_commit_flag (commit_tran_flag);

    return NO_ERROR;
  }

  int log_generator::append_repl_object (replication_object *object)
  {
    m_stream_entry.add_packable_entry (object);

    if (enable_log_generator_logging)
      {
	object->log_me ("from append_repl_object(replication_object *)");
      }

    return NO_ERROR;
  }

  void log_generator::append_pending_repl_object (changed_attrs_row_repl_entry *object)
  {
    m_pending_to_be_added.push_back (object);
  }

  int log_generator::append_pending_repl_object (cubthread::entry &thread_entry, const OID *class_oid,
      const OID *inst_oid, ATTR_ID col_id, DB_VALUE *value)
  {
    changed_attrs_row_repl_entry *entry = NULL;
    char *class_name = NULL;

    if (inst_oid == NULL)
      {
	return NO_ERROR;
      }

    for (auto &repl_obj : m_pending_to_be_added)
      {
	if (repl_obj->compare_inst_oid (inst_oid))
	  {
	    entry = repl_obj;
	    break;
	  }
      }

    if (entry != NULL)
      {
	entry->copy_and_add_changed_value (thread_entry,
					   col_id,
					   value);
      }
    else
      {
	int error_code = NO_ERROR;

	if (heap_get_class_name (&thread_entry, class_oid, &class_name) != NO_ERROR || class_name == NULL)
	  {
	    ASSERT_ERROR_AND_SET (error_code);
	    if (error_code == NO_ERROR)
	      {
		error_code = ER_REPL_ERROR;
		er_set (ER_WARNING_SEVERITY, ARG_FILE_LINE, ER_REPL_ERROR, 1, "can't get class_name");
	      }
	    assert (false);
	    return error_code;
	  }

	entry = new changed_attrs_row_repl_entry (cubreplication::REPL_ENTRY_TYPE::REPL_UPDATE,
	    class_name,
	    inst_oid);
	entry->copy_and_add_changed_value (thread_entry,
					   col_id,
					   value);

	m_pending_to_be_added.push_back (entry);
      }

    if (enable_log_generator_logging)
      {
	entry->log_me ("from append_pending_repl_object(*)");
      }

    return NO_ERROR;
  }

  int log_generator::set_key_to_repl_object (cubthread::entry &thread_entry, DB_VALUE *key, const OID *inst_oid,
      char *class_name, RECDES *optional_recdes)
  {
    bool found = false;

    assert (inst_oid != NULL);

    for (auto repl_obj_it = m_pending_to_be_added.begin ();
	 repl_obj_it != m_pending_to_be_added.end (); ++repl_obj_it)
      {
	if ((*repl_obj_it)->compare_inst_oid (inst_oid))
	  {
	    (*repl_obj_it)->set_key_value (thread_entry, key);

	    (void) log_generator::append_repl_object (*repl_obj_it);
	    repl_obj_it = m_pending_to_be_added.erase (repl_obj_it);

	    found = true;

	    break;
	  }
      }

    if (!found)
      {
	if (optional_recdes == NULL)
	  {
	    assert (false);
	    return ER_FAILED;
	  }

	cubreplication::rec_des_row_repl_entry *entry = new cubreplication::rec_des_row_repl_entry (
	  cubreplication::REPL_ENTRY_TYPE::REPL_UPDATE,
	  class_name,
	  optional_recdes);

	(void) log_generator::append_repl_object (entry);
      }

    return NO_ERROR;
  }

  stream_entry *log_generator::get_stream_entry (void)
  {
    return &m_stream_entry;
  }

  int log_generator::pack_stream_entry (void)
  {
    m_stream_entry.pack ();
    m_stream_entry.reset ();
    m_stream_entry.set_commit_flag (false);

    return NO_ERROR;
  }

  void log_generator::pack_group_commit_entry (void)
  {
    static stream_entry gc_stream_entry (g_stream, MVCCID_NULL, true, true);
    gc_stream_entry.pack ();
  }

  int log_generator::create_stream (const cubstream::stream_position &start_position)
  {
    log_generator::g_start_append_position = start_position;

    /* TODO : stream should be created by a high level object together with log_generator */
    /* create stream only for global instance */
    INT64 buffer_size = prm_get_bigint_value (PRM_ID_REPL_GENERATOR_BUFFER_SIZE);
    int num_max_appenders = log_Gl.trantable.num_total_indices + 1;

    log_generator::g_stream = new cubstream::multi_thread_stream (buffer_size, num_max_appenders);
    log_generator::g_stream->set_trigger_min_to_read_size (stream_entry::compute_header_size ());
    log_generator::g_stream->init (log_generator::g_start_append_position);

    for (int i = 0; i < log_Gl.trantable.num_total_indices; i++)
      {
	LOG_TDES *tdes = LOG_FIND_TDES (i);

	log_generator *lg = & (tdes->replication_log_generator);

	lg->m_stream_entry.set_stream (log_generator::g_stream);
      }

    return NO_ERROR;
  }

  cubstream::multi_thread_stream *log_generator::g_stream = NULL;

  cubstream::stream_position log_generator::g_start_append_position = 0;

} /* namespace cubreplication */
