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
 * log_writer.h - DECLARATIONS FOR LOG WRITER (AT CLIENT & SERVER)
 */

#ifndef _LOG_WRITER_HEADER_
#define _LOG_WRITER_HEADER_

#ident "$Id$"

#include "client_credentials.hpp"
#include "log_archives.hpp"
#include "log_common_impl.h"
#include "log_lsa.hpp"
#include "storage_common.h"
#if defined (SERVER_MODE)
#include "thread_compat.hpp"
#endif // SERVER_MODE

#include <stdio.h>

typedef struct logwr_context LOGWR_CONTEXT;
struct logwr_context
{
  int rc;
  int last_error;
  bool shutdown;
};

enum logwr_mode
{
  LOGWR_MODE_ASYNC = 1,
  LOGWR_MODE_SEMISYNC,
  LOGWR_MODE_SYNC
};
typedef enum logwr_mode LOGWR_MODE;
#define LOGWR_COPY_FROM_FIRST_PHY_PAGE_MASK	(0x80000000)

#if defined(CS_MODE)
enum logwr_action
{
  LOGWR_ACTION_NONE = 0x00,
  LOGWR_ACTION_DELAYED_WRITE = 0x01,
  LOGWR_ACTION_ASYNC_WRITE = 0x02,
  LOGWR_ACTION_HDR_WRITE = 0x04,
  LOGWR_ACTION_ARCHIVING = 0x08
};
typedef enum logwr_action LOGWR_ACTION;

typedef struct logwr_global LOGWR_GLOBAL;
struct logwr_global
{
  LOG_HEADER hdr;
  LOG_PAGE *loghdr_pgptr;

  char db_name[PATH_MAX];
  char *hostname;
  char log_path[PATH_MAX];
  char loginf_path[PATH_MAX];
  char active_name[PATH_MAX];
  int append_vdes;

  char *logpg_area;
  int logpg_area_size;
  int logpg_fill_size;

  LOG_PAGE **toflush;
  int max_toflush;
  int num_toflush;

  LOGWR_MODE mode;
  LOGWR_ACTION action;

  LOG_PAGEID last_chkpt_pageid;
  LOG_PAGEID last_recv_pageid;
  LOG_PAGEID last_arv_fpageid;
  LOG_PAGEID last_arv_lpageid;
  int last_arv_num;

  bool force_flush;
  struct timeval last_flush_time;
  /* background log archiving info */
  BACKGROUND_ARCHIVING_INFO bg_archive_info;
  char bg_archive_name[PATH_MAX];

  /* original next logical page to archive */
  LOG_PAGEID ori_nxarv_pageid;

  /* start pageid */
  LOG_PAGEID start_pageid;

  bool reinit_copylog;
};

#define LOGWR_AT_NEXT_ARCHIVE_PAGE_ID(pageid) \
        (logwr_to_physical_pageid(pageid) == logwr_Gl.hdr.nxarv_phy_pageid)

#define LOGWR_IS_ARCHIVE_PAGE(pageid) \
        ((pageid) != LOGPB_HEADER_PAGE_ID && (pageid) < logwr_Gl.hdr.nxarv_pageid)

#define LOGWR_AT_SERVER_ARCHIVING() \
	(LOGWR_AT_NEXT_ARCHIVE_PAGE_ID(logwr_Gl.hdr.append_lsa.pageid) \
	 && (logwr_Gl.hdr.eof_lsa.pageid < logwr_Gl.hdr.append_lsa.pageid))

extern LOGWR_GLOBAL logwr_Gl;
extern void logwr_flush_header_page (void);
extern int logwr_write_log_pages (void);
extern int logwr_set_hdr_and_flush_info (void);
#if !defined(WINDOWS)
extern int logwr_copy_log_header_check (const char *db_name, bool verbose, LOG_LSA * master_eof_lsa);
#endif /* !WINDOWS */
#else /* !CS_MODE = SERVER_MODE || SA_MODE */
enum logwr_status
{
  LOGWR_STATUS_WAIT,
  LOGWR_STATUS_FETCH,
  LOGWR_STATUS_DONE,
  LOGWR_STATUS_DELAY,
  LOGWR_STATUS_ERROR
};
typedef enum logwr_status LOGWR_STATUS;

typedef struct logwr_entry LOGWR_ENTRY;
struct logwr_entry
{
  THREAD_ENTRY *thread_p;
  LOG_PAGEID fpageid;
  LOGWR_MODE mode;
  LOGWR_STATUS status;
  LOG_LSA eof_lsa;
  LOG_LSA last_sent_eof_lsa;
  LOG_LSA tmp_last_sent_eof_lsa;
  INT64 start_copy_time;
  bool copy_from_first_phy_page;
  LOGWR_ENTRY *next;
};

typedef struct logwr_info LOGWR_INFO;
struct logwr_info
{
  LOGWR_ENTRY *writer_list;
  pthread_mutex_t wr_list_mutex;
  pthread_cond_t flush_start_cond;
  pthread_mutex_t flush_start_mutex;
  pthread_cond_t flush_wait_cond;
  pthread_mutex_t flush_wait_mutex;
  pthread_cond_t flush_end_cond;
  pthread_mutex_t flush_end_mutex;
  bool skip_flush;
  bool flush_completed;
  bool is_init;

  /* to measure the time spent by the last LWT delaying LFT */
  bool trace_last_writer;
  CLIENTIDS last_writer_client_info;
  INT64 last_writer_elapsed_time;

  // *INDENT-OFF*
  logwr_info ()
    : writer_list (NULL)
    , wr_list_mutex PTHREAD_MUTEX_INITIALIZER
    , flush_start_cond PTHREAD_COND_INITIALIZER
    , flush_start_mutex PTHREAD_MUTEX_INITIALIZER
    , flush_wait_cond PTHREAD_COND_INITIALIZER
    , flush_wait_mutex PTHREAD_MUTEX_INITIALIZER
    , flush_end_cond PTHREAD_COND_INITIALIZER
    , flush_end_mutex PTHREAD_MUTEX_INITIALIZER
    , skip_flush (false)
    , flush_completed (false)
    , is_init (false)
    , trace_last_writer (false)
    , last_writer_client_info ()
    , last_writer_elapsed_time (0)
  {
  }
  // *INDENT-ON*
};
#endif /* !CS_MODE = SERVER_MODE || SA_MODE */

extern bool logwr_force_shutdown (void);
extern int logwr_copy_log_file (const char *db_name, const char *log_path, int mode, INT64 start_page_id);
extern LOG_PHY_PAGEID logwr_to_physical_pageid (LOG_PAGEID logical_pageid);
extern const char *logwr_log_ha_filestat_to_string (enum LOG_HA_FILESTAT val);

#if defined(SERVER_MODE)
int xlogwr_get_log_pages (THREAD_ENTRY * thread_p, LOG_PAGEID first_pageid, LOGWR_MODE mode);
extern LOG_PAGEID logwr_get_min_copied_fpageid (void);

#endif /* SERVER_MODE */
#endif /* _LOG_WRITER_HEADER_ */
