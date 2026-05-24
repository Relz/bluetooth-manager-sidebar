#include "cli/cli.h"

#include <stdio.h>
#include <string.h>

#include "app/app-metadata.h"

typedef struct {
  const char *option;
  SidebarCommand command;
} CommandOption;

static const CommandOption command_options[] = {
  { "--toggle", SIDEBAR_COMMAND_TOGGLE },
  { "--show", SIDEBAR_COMMAND_SHOW },
  { "--hide", SIDEBAR_COMMAND_HIDE },
  { "--quit", SIDEBAR_COMMAND_QUIT },
  { "--reload-css", SIDEBAR_COMMAND_RELOAD_CSS },
  { "--background", SIDEBAR_COMMAND_BACKGROUND },
};

static char *
cli_program_name(const char *program_name)
{
  if (program_name && *program_name)
    return g_path_get_basename(program_name);
  return g_strdup(COMMAND_NAME);
}

static char *
cli_usage_error(const char *program, const char *detail)
{
  return g_strdup_printf("usage: %s [-h] [--toggle | --show | --hide | --quit |\n"
                         "                       --reload-css | --background]\n"
                         "%s: error: %s",
                         program,
                         program,
                         detail);
}

gboolean
cli_parse(int argc,
              char **argv,
              SidebarCommand *out_command,
              gboolean *out_help_requested,
              GError **error)
{
  SidebarCommand command = SIDEBAR_COMMAND_TOGGLE;
  gboolean command_seen = FALSE;
  const char *selected_option = NULL;
  g_autofree char *program = NULL;

  g_return_val_if_fail(out_command != NULL, FALSE);
  g_return_val_if_fail(out_help_requested != NULL, FALSE);

  *out_help_requested = FALSE;
  program = cli_program_name(argc > 0 && argv ? argv[0] : NULL);

  for (int i = 1; i < argc; i++) {
    const char *arg = argv[i];
    gboolean matched = FALSE;

    if (g_strcmp0(arg, "--help") == 0 || g_strcmp0(arg, "-h") == 0) {
      *out_help_requested = TRUE;
      *out_command = command;
      return TRUE;
    }

    for (guint j = 0; j < G_N_ELEMENTS(command_options); j++) {
      if (g_strcmp0(arg, command_options[j].option) == 0) {
        if (command_seen) {
          g_autofree char *detail = g_strdup_printf("argument %s: not allowed with argument %s",
                                                    arg,
                                                    selected_option);
          g_autofree char *message = cli_usage_error(program, detail);

          g_set_error_literal(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED, message);
          return FALSE;
        }
        command = command_options[j].command;
        command_seen = TRUE;
        selected_option = arg;
        matched = TRUE;
        break;
      }
    }

    if (!matched) {
      g_autofree char *detail = g_strdup_printf("unrecognized arguments: %s", arg);
      g_autofree char *message = cli_usage_error(program, detail);

      g_set_error_literal(error, G_OPTION_ERROR, G_OPTION_ERROR_UNKNOWN_OPTION, message);
      return FALSE;
    }
  }

  *out_command = command;
  return TRUE;
}

void
cli_print_help(const char *program_name)
{
  g_autofree char *prog = cli_program_name(program_name);

  g_print("usage: %s [-h] [--toggle | --show | --hide | --quit |\n", prog);
  g_print("                       --reload-css | --background]\n\n");
  g_print("%s for Wayland desktops\n\n", APP_NAME);
  g_print("options:\n");
  g_print("  -h, --help      show this help message and exit\n");
  g_print("  --toggle        toggle sidebar\n");
  g_print("  --show          show sidebar\n");
  g_print("  --hide          hide sidebar\n");
  g_print("  --quit          quit the background process\n");
  g_print("  --reload-css    reload user CSS in the running instance\n");
  g_print("  --background    verify an existing instance or start one without showing the\n");
  g_print("                  sidebar\n\n");
  g_print("Running without options defaults to --toggle.\n");
}
