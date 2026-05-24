#include "command/command-codec.h"

#include <string.h>

#include "core/target-output.h"

static char *
command_ipc_normalize_text(const char *text)
{
  g_autofree char *copy = NULL;

  if (!text || !g_utf8_validate(text, -1, NULL))
    return NULL;

  copy = g_strdup(text);
  g_strstrip(copy);
  return g_utf8_strdown(copy, -1);
}

char *
command_ipc_encode(SidebarCommand command, const char *target_output_name)
{
  g_autofree char *target = target_output_normalize(target_output_name);

  if (target)
    return g_strdup_printf("%s\t%s\n", sidebar_command_to_string(command), target);
  return g_strdup_printf("%s\n", sidebar_command_to_string(command));
}

gboolean
command_ipc_decode_line(const char *line,
                        SidebarCommand *out_command,
                        char **out_target_output_name)
{
  const char *separator;
  g_autofree char *command_text = NULL;

  g_return_val_if_fail(out_command != NULL, FALSE);
  g_return_val_if_fail(out_target_output_name != NULL, FALSE);

  *out_target_output_name = NULL;

  if (!line || !*line || !g_utf8_validate(line, -1, NULL))
    return FALSE;

  separator = strchr(line, '\t');
  if (separator) {
    g_autofree char *raw_command = g_strndup(line, separator - line);
    command_text = command_ipc_normalize_text(raw_command);
    if (!command_text)
      return FALSE;
    *out_target_output_name = target_output_normalize(separator + 1);
  } else {
    command_text = command_ipc_normalize_text(line);
    if (!command_text)
      return FALSE;
  }

  if (!*command_text)
    return FALSE;

  return sidebar_command_parse_text(command_text, out_command) && sidebar_command_is_socket_command(*out_command);
}

char *
command_ipc_success_response(SidebarCommand command)
{
  return g_strdup_printf("%s ok %s\n", IPC_PROTOCOL, sidebar_command_to_string(command));
}

gboolean
command_ipc_response_is_success(const char *response, SidebarCommand command)
{
  g_autofree char *expected = command_ipc_success_response(command);

  return g_strcmp0(response, expected) == 0;
}
