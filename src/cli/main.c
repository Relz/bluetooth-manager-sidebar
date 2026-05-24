#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "app/app-metadata.h"
#include "cli/cli.h"
#include "command/startup-command-router.h"
#include "core/config.h"

static char *gui_path_from_argv0(const char *argv0);
static int run_application_command(const char *argv0, SidebarCommand command);

int
main(int argc, char **argv)
{
  SidebarCommand command = SIDEBAR_COMMAND_TOGGLE;
  gboolean help_requested = FALSE;
  StartupCommandRoute route = { 0 };
  g_autoptr(GError) error = NULL;

  if (!cli_parse(argc, argv, &command, &help_requested, &error)) {
    g_printerr("%s\n", error ? error->message : "bm-sidebar: error: invalid arguments");
    return 2;
  }

  if (help_requested) {
    cli_print_help(argv[0]);
    return 0;
  }

  route = startup_command_route(command);
  if (!route.should_launch)
    return route.exit_status;

  return run_application_command(argv[0], command);
}

static char *
gui_path_from_argv0(const char *argv0)
{
  g_autofree char *dirname = NULL;
  g_autofree char *candidate = NULL;

  if (argv0 && strchr(argv0, G_DIR_SEPARATOR) != NULL) {
    dirname = g_path_get_dirname(argv0);
    candidate = g_build_filename(dirname, "bm-sidebar-gui", NULL);
    if (g_file_test(candidate, G_FILE_TEST_IS_EXECUTABLE))
      return g_steal_pointer(&candidate);
  }

  if (g_file_test(INSTALLED_GUI_PATH, G_FILE_TEST_IS_EXECUTABLE))
    return g_strdup(INSTALLED_GUI_PATH);

  return g_strdup("bm-sidebar-gui");
}

static int
run_application_command(const char *argv0, SidebarCommand command)
{
  g_autofree char *gui_path = gui_path_from_argv0(argv0);
  char *const child_argv[] = { gui_path, (char *)sidebar_command_to_string(command), NULL };

  execv(gui_path, child_argv);

  if (errno == ENOENT && strchr(gui_path, G_DIR_SEPARATOR) == NULL)
    execvp(gui_path, child_argv);

  g_printerr("%s: could not start GUI helper %s: %s\n", COMMAND_NAME, gui_path, g_strerror(errno));
  return 1;
}
