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
 * replication_stream_entry.hpp
 */

#ident "$Id$"

#ifndef _REPLICATION_STREAM_ENTRY_HPP_
#define _REPLICATION_STREAM_ENTRY_HPP_

#include "stream_entry.hpp"
#include "replication_entry.hpp"
#include "storage_common.h"
#include <vector>

namespace cubreplication
{

  /* a replication stream entry is generated by a transaction or a group commit action */
  struct replication_stream_entry_header
  {
    const static unsigned int COMMIT_FLAG = 0x80000000;
    const static unsigned int GROUP_COMMIT_FLAG = 0x40000000;

    const static unsigned int COUNT_VALUE_MASK = ~ (COMMIT_FLAG | GROUP_COMMIT_FLAG);

    cubstream::stream_position prev_record;
    MVCCID mvccid;
    unsigned int count_replication_entries;
    int data_size;

    /* flags is packed into 'count_replication_entries' field */
    /* commit_flag : transaction is committed */
    bool commit_flag;
    /* group_commit_flag : a group of transaction were committed */
    bool group_commit_flag;

    replication_stream_entry_header ()
      : prev_record (0),
	mvccid (MVCCID_NULL),
	count_replication_entries (0),
	commit_flag (false),
	group_commit_flag (false)
    {
    };
  };

  class replication_stream_entry : public cubstream::entry<replication_object>
  {
    private:
      replication_stream_entry_header m_header;
      cubpacking::packer m_serializator;

    public:
      replication_stream_entry (cubstream::packing_stream *stream_p)
	: entry (stream_p)
      {
      };

      replication_stream_entry (cubstream::packing_stream *stream_p,
				MVCCID arg_mvccid,
				bool arg_commit_flag,
				bool arg_group_commit_flag)
	: entry (stream_p)
      {
	m_header.mvccid = arg_mvccid;
	m_header.commit_flag = arg_commit_flag;
	m_header.group_commit_flag = arg_group_commit_flag;
      };

      size_t get_header_size ();
      size_t get_data_packed_size (void);
      void set_header_data_size (const size_t &data_size);

      cubstream::entry<replication_object>::packable_factory *get_builder ();

      cubpacking::packer *get_packer ()
      {
	return &m_serializator;
      }

      void set_mvccid (MVCCID mvccid)
      {
	m_header.mvccid = mvccid;
      }

      MVCCID get_mvccid ()
      {
	return m_header.mvccid;
      }

      void set_commit_flag (bool commit)
      {
	m_header.commit_flag = commit;
      }

      bool is_group_commit (void)
      {
	return m_header.group_commit_flag;
      }

      bool is_tran_commit (void)
      {
	return m_header.commit_flag;
      }

      int pack_stream_entry_header ();
      int unpack_stream_entry_header ();
      int get_packable_entry_count_from_header (void);

      bool is_equal (const cubstream::entry<replication_object> *other);
  };

} /* namespace cubreplication */

#endif /* _REPLICATION_STREAM_ENTRY_HPP_ */
