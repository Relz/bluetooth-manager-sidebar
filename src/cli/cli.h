#pragma once

#include <glib.h>

#include "command/sidebar-command.h"

gboolean cli_parse(int argc,
                       char **argv,
                       SidebarCommand *out_command,
                       gboolean *out_help_requested,
                       GError **error);

void cli_print_help(const char *program_name);
