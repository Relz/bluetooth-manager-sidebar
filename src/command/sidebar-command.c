#include "command/sidebar-command.h"

typedef struct {
  const char *text;
  SidebarCommand command;
} SidebarCommandName;

static const SidebarCommandName command_names[] = {
  { "toggle", SIDEBAR_COMMAND_TOGGLE },
  { "show", SIDEBAR_COMMAND_SHOW },
  { "hide", SIDEBAR_COMMAND_HIDE },
  { "quit", SIDEBAR_COMMAND_QUIT },
  { "reload-css", SIDEBAR_COMMAND_RELOAD_CSS },
  { "background", SIDEBAR_COMMAND_BACKGROUND },
  { "ping", SIDEBAR_COMMAND_PING },
};

const char *
sidebar_command_to_string(SidebarCommand command)
{
  for (guint i = 0; i < G_N_ELEMENTS(command_names); i++) {
    if (command_names[i].command == command)
      return command_names[i].text;
  }

  return "toggle";
}

gboolean
sidebar_command_parse_text(const char *text, SidebarCommand *out_command)
{
  g_autofree char *normalized = g_utf8_strdown(text ? text : "", -1);

  g_return_val_if_fail(out_command != NULL, FALSE);

  g_strstrip(normalized);
  if (!*normalized)
    return FALSE;

  for (guint i = 0; i < G_N_ELEMENTS(command_names); i++) {
    if (g_strcmp0(normalized, command_names[i].text) == 0) {
      *out_command = command_names[i].command;
      return TRUE;
    }
  }

  return FALSE;
}

gboolean
sidebar_command_starts_hidden(SidebarCommand command)
{
  return command == SIDEBAR_COMMAND_BACKGROUND;
}

gboolean
sidebar_command_is_ipc_only(SidebarCommand command)
{
  return command == SIDEBAR_COMMAND_HIDE ||
         command == SIDEBAR_COMMAND_QUIT ||
         command == SIDEBAR_COMMAND_RELOAD_CSS;
}

gboolean
sidebar_command_is_socket_command(SidebarCommand command)
{
  return command == SIDEBAR_COMMAND_TOGGLE ||
         command == SIDEBAR_COMMAND_SHOW ||
         command == SIDEBAR_COMMAND_HIDE ||
         command == SIDEBAR_COMMAND_QUIT ||
         command == SIDEBAR_COMMAND_RELOAD_CSS ||
         command == SIDEBAR_COMMAND_PING;
}
