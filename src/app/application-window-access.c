#include "app/application-window-access.h"

#include "app/application-layer-shell-policy.h"
#include "app/application-private.h"
#include "app/application-window-callbacks.h"
#include "app/application-window-initial-render.h"
#include "sidebar/sidebar-window.h"

static GtkWindow *
application_window_create(Application *self)
{
  SidebarWindowCallbacks callbacks = application_window_callbacks(self);

  return sidebar_window_new(GTK_APPLICATION(self), &callbacks, self->target_output_name);
}

static void
application_window_track(Application *self, GtkWindow *window)
{
  self->window = window;
  g_object_add_weak_pointer(G_OBJECT(self->window), (gpointer *)&self->window);
}

GtkWindow *
application_window(Application *self)
{
  g_return_val_if_fail(IS_APPLICATION(self), NULL);

  return self->window;
}

GtkWindow *
application_ensure_window(Application *self)
{
  GtkWindow *window = NULL;

  if (self->window)
    return self->window;

  if (!application_layer_shell_policy_require_supported(self))
    return NULL;

  window = application_window_create(self);
  if (!window)
    return NULL;

  application_window_track(self, window);
  application_window_initial_render(self);

  return self->window;
}
