#include <adwaita.h>

#include "app/application.h"
#include "app/app-metadata.h"

int
main(int argc, char **argv)
{
  SidebarCommand command = SIDEBAR_COMMAND_TOGGLE;
  g_autoptr(GError) error = NULL;

  if (!application_command_from_arguments(argv, argc, &command, &error)) {
    g_printerr("%s: error: %s\n", COMMAND_NAME, error ? error->message : "invalid arguments");
    return 2;
  }

  g_autoptr(Application) app = application_new(sidebar_command_starts_hidden(command));
  int status = g_application_run(G_APPLICATION(app), argc, argv);

  return (application_get_unsupported_layer_shell(app) || application_get_runtime_failure(app)) ? 1 : status;
}
