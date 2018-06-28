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
#include "master_replication_channel.hpp"
#include "thread_entry.hpp"
#include "packing_stream.hpp"
#include "connection_globals.h"
#include "log_impl.h"

namespace cubreplication
{
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

    return NO_ERROR;
  }

  replication_stream_entry *log_generator::get_stream_entry (void)
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

  int log_generator::pack_group_commit_entry (void)
  {
    static replication_stream_entry gc_stream_entry (g_stream, MVCCID_NULL, true, true);
    gc_stream_entry.pack ();

    return NO_ERROR;
  }

  int log_generator::create_stream (const cubstream::stream_position &start_position)
  {
    log_generator::g_start_append_position = start_position;

    /* create stream only for global instance */
    INT64 buffer_size = prm_get_bigint_value (PRM_ID_REPL_GENERATOR_BUFFER_SIZE);
    int num_max_appenders = log_Gl.trantable.num_total_indices + 1;

    log_generator::g_stream = new cubstream::packing_stream (buffer_size, num_max_appenders);
    log_generator::g_stream->init (log_generator::g_start_append_position);

    for (int i = 0; i < log_Gl.trantable.num_total_indices; i++)
      {
        LOG_TDES *tdes = LOG_FIND_TDES (i);

        log_generator *lg = &(tdes->replication_log_generator);

	lg->m_stream_entry.set_stream (log_generator::g_stream);
      }

    return NO_ERROR;
  }

  cubstream::packing_stream* log_generator::g_stream = NULL;

  cubstream::stream_position log_generator::g_start_append_position = 0;

} /* namespace cubreplication */
