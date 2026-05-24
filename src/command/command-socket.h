#pragma once

#include <gio/gio.h>
#include <glib.h>
#include <sys/types.h>

#include "command/sidebar-command.h"

#define COMMAND_SOCKET_IO_TIMEOUT_SECONDS 1
#define COMMAND_SOCKET_PROBE_TIMEOUT_SECONDS 0.5

typedef enum {
  COMMAND_SOCKET_PROBE_ACKNOWLEDGED,
  COMMAND_SOCKET_PROBE_UNACKNOWLEDGED,
  COMMAND_SOCKET_PROBE_NOT_CONNECTABLE,
} CommandSocketProbeStatus;

CommandSocketProbeStatus command_socket_probe(const char *path);
gboolean command_socket_acknowledges_ping(const char *path);
gboolean command_socket_send_all(GSocket *socket, const char *payload);
void command_socket_send_success(GSocket *socket, SidebarCommand command);
gboolean command_socket_read_command(GSocket *connection,
                                     SidebarCommand *out_command,
                                     char **out_target_output_name);
const char *command_socket_file_type_name(mode_t mode);
