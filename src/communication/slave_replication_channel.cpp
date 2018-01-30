#include "slave_replication_channel.hpp"

#include "connection_globals.h"
#include "connection_sr.h"
#include "thread_manager.hpp"
#include "thread_entry_task.hpp"
#include "thread_looper.hpp"
#include "system_parameter.h"

slave_replication_channel *slave_replication_channel::singleton = NULL;
std::mutex slave_replication_channel::singleton_mutex;

class slave_dummy_send_msg : public cubthread::entry_task
{
  public:
    slave_dummy_send_msg (slave_replication_channel *ch) : channel (ch)
    {
      if (GETHOSTNAME (this_hostname, MAXHOSTNAMELEN) != 0)
	{
	  _er_log_debug (ARG_FILE_LINE, "unable to find this computer's hostname\n");
          strcpy (this_hostname, "unknown");
	}
      this_hostname[MAXHOSTNAMELEN-1] = '\0';
    }

    void execute (cubthread::entry &context)
    {
      if (!IS_INVALID_SOCKET (channel->get_master_conn_entry()->fd))
        {
          int rc = channel->send (channel->get_master_conn_entry()->fd, std::string ("hello from ") + this_hostname, communication_channel::get_max_timeout());
          if (rc == ERROR_ON_WRITE)
            {
              /* this probably means that the connection was closed */
              css_free_conn (channel->get_master_conn_entry());
              slave_replication_channel::reset_singleton();
            }
          else if (rc != NO_ERRORS)
            {
              assert (false);
            }
          _er_log_debug (ARG_FILE_LINE, "slave::execute:" "sent:hello\n");
        }
    }

    void retire ()
    {

    }
  private:
    slave_replication_channel *channel;
    char this_hostname[MAXHOSTNAMELEN];
};

slave_replication_channel::slave_replication_channel(const std::string& hostname, const std::string &master_server_name, int port) : master_hostname (hostname),
                                                                                                                                     master_server_name (master_server_name),
                                                                                                                                     master_port (port)
{
  master_conn_entry = css_make_conn (-1);
  int rc = rmutex_initialize (&master_conn_entry->rmutex, "MASTER_CONN_ENTRY");
  assert (rc == NO_ERROR);
  request_id = -1;

  _er_log_debug (ARG_FILE_LINE, "init slave_replication_channel \n");
}

slave_replication_channel::~slave_replication_channel()
{
  cubthread::get_manager()->destroy_daemon (slave_dummy);
  css_free_conn (master_conn_entry);

  _er_log_debug (ARG_FILE_LINE, "destroy slave_replication_channel \n");
}

int slave_replication_channel::connect_to_master()
{
  int length = 0;

  if (master_conn_entry == NULL)
    {
      return ER_FAILED;
    }

  length = master_server_name.length ();

  if (css_common_connect (master_conn_entry, &request_id, master_hostname.c_str (),
                          SERVER_REQUEST_CONNECT_NEW_SLAVE, master_server_name.c_str (), length, master_port) == NULL)
    {
      assert (false);
      return REQUEST_REFUSED;
    }

  _er_log_debug (ARG_FILE_LINE, "connect_to_master:" "connected to master_hostname:%s\n", master_hostname.c_str ());

  return NO_ERRORS;
}

CSS_CONN_ENTRY *slave_replication_channel::get_master_conn_entry ()
{
  return master_conn_entry;
}

void slave_replication_channel::init (const std::string &hostname, const std::string &server_name, int port)
{
  if (singleton == NULL)
    {
      std::lock_guard<std::mutex> guard (singleton_mutex);
      if (singleton == NULL)
        {
          singleton = new slave_replication_channel (hostname, server_name, port);
        }
    }
}

void slave_replication_channel::reset_singleton ()
{
  delete singleton;
  singleton = NULL;
}

slave_replication_channel *slave_replication_channel::get_channel ()
{
  return singleton;
}

int slave_replication_channel::start_daemon ()
{
  cubthread::manager *session_manager = cubthread::get_manager ();
  slave_dummy = session_manager->create_daemon (cubthread::looper (std::chrono::seconds (1)),
		       new slave_dummy_send_msg (this));

  if (slave_dummy == NULL)
    {
      return ER_FAILED;
    }

  return NO_ERROR;
}
