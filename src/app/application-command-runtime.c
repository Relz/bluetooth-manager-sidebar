#include "app/application-command-runtime.h"

#include "app/application-commands.h"
#include "command/command-ipc.h"

struct _ApplicationCommandRuntime {
  CommandServer *server;
};

ApplicationCommandRuntime *
application_command_runtime_new(void)
{
  return g_new0(ApplicationCommandRuntime, 1);
}

gboolean
application_command_runtime_start(ApplicationCommandRuntime *runtime,
                                  Application *self,
                                  GError **error)
{
  g_return_val_if_fail(runtime != NULL, FALSE);

  g_clear_pointer(&runtime->server, command_server_free);
  runtime->server = command_server_new(application_commands_handle_ipc, self, NULL, error);
  return runtime->server != NULL;
}

gboolean
application_command_runtime_ensure_healthy(ApplicationCommandRuntime *runtime,
                                           Application *self,
                                           GError **error)
{
  g_return_val_if_fail(runtime != NULL, FALSE);

  if (command_server_is_healthy(runtime->server))
    return TRUE;

  return application_command_runtime_start(runtime, self, error);
}

void
application_command_runtime_free(ApplicationCommandRuntime *runtime)
{
  if (!runtime)
    return;

  g_clear_pointer(&runtime->server, command_server_free);
  g_free(runtime);
}
