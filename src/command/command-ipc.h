#pragma once

#include <glib.h>

#include "command/command-codec.h"
#include "command/command-socket-path.h"
#include "command/sidebar-command.h"

typedef struct _CommandServer CommandServer;

typedef enum {
  COMMAND_IPC_NOT_DELIVERED,
  COMMAND_IPC_DELIVERED,
  COMMAND_IPC_ACKNOWLEDGED,
} CommandIpcStatus;

typedef void (*CommandHandler)(SidebarCommand command,
                                  const char *target_output_name,
                                  gpointer user_data);

CommandIpcStatus command_ipc_send(SidebarCommand command,
                                     const char *target_output_name,
                                     GError **error);

CommandServer *command_server_new(CommandHandler handler,
                                          gpointer user_data,
                                          GDestroyNotify destroy,
                                          GError **error);
gboolean command_server_is_healthy(CommandServer *server);
void command_server_close(CommandServer *server);
void command_server_free(CommandServer *server);
