#include "app/application.h"

#include "app/application-pages.h"
#include "app/application-window-access.h"

#include "app/application-private.h"
#include "sidebar/sidebar-window.h"

void
application_show_sidebar(Application *self)
{
  GtkWindow *window;

  g_return_if_fail(IS_APPLICATION(self));

  window = application_ensure_window(self);
  if (window) {
    sidebar_window_set_target_output(window, self->target_output_name);
    if (self->refresh_dirty)
      application_refresh_now(self);
    gtk_window_present(window);
  }
}

void
application_hide_sidebar(Application *self)
{
  GtkWindow *window = NULL;

  g_return_if_fail(IS_APPLICATION(self));

  window = application_window(self);
  if (window)
    gtk_widget_set_visible(GTK_WIDGET(window), FALSE);
}

void
application_toggle_sidebar(Application *self)
{
  GtkWindow *window;

  g_return_if_fail(IS_APPLICATION(self));

  window = application_ensure_window(self);
  if (!window)
    return;

  if (gtk_widget_get_visible(GTK_WIDGET(window)))
    application_hide_sidebar(self);
  else
    application_show_sidebar(self);
}
