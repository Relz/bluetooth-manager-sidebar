#pragma once

#include <glib.h>

typedef enum {
  SIDEBAR_COMMAND_TOGGLE,
  SIDEBAR_COMMAND_SHOW,
  SIDEBAR_COMMAND_HIDE,
  SIDEBAR_COMMAND_QUIT,
  SIDEBAR_COMMAND_RELOAD_CSS,
  SIDEBAR_COMMAND_BACKGROUND,
  SIDEBAR_COMMAND_PING,
} SidebarCommand;

const char *sidebar_command_to_string(SidebarCommand command);
gboolean sidebar_command_parse_text(const char *text, SidebarCommand *out_command);
gboolean sidebar_command_starts_hidden(SidebarCommand command);
gboolean sidebar_command_is_ipc_only(SidebarCommand command);
gboolean sidebar_command_is_socket_command(SidebarCommand command);
