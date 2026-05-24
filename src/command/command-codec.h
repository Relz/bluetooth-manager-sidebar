#pragma once

#include <gio/gio.h>

#include "command/sidebar-command.h"

#define IPC_PROTOCOL "bm-sidebar-ipc-v1"
#define IPC_MAX_COMMAND_BYTES 512
#define IPC_MAX_RESPONSE_BYTES 128

char *command_ipc_encode(SidebarCommand command, const char *target_output_name);
gboolean command_ipc_decode_line(const char *line,
                                SidebarCommand *out_command,
                                char **out_target_output_name);
char *command_ipc_success_response(SidebarCommand command);
gboolean command_ipc_response_is_success(const char *response, SidebarCommand command);
