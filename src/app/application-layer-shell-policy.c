#include "app/application-layer-shell-policy.h"

#include "app/application-private.h"
#include "app/app-metadata.h"
#include "sidebar/sidebar-window.h"
#include "core/ui-constants.h"

gboolean
application_layer_shell_policy_require_supported(Application *self)
{
  if (!sidebar_window_is_supported()) {
    g_printerr("%s: %s (the current compositor/session does not support gtk4-layer-shell)\n",
               COMMAND_NAME,
               LAYER_SHELL_REQUIRED_MESSAGE);
    self->unsupported_layer_shell = TRUE;
    g_application_quit(G_APPLICATION(self));
    return FALSE;
  }

  return TRUE;
}
