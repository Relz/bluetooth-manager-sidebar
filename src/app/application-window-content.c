#include "app/application-window-access.h"

#include "sidebar/sidebar-window.h"

void
application_show_toast(Application *self, const char *message)
{
  GtkWindow *window = NULL;

  g_return_if_fail(IS_APPLICATION(self));

  window = application_window(self);
  if (window)
    sidebar_window_show_toast(window, message);
}

void
application_append_content(Application *self, GtkWidget *child)
{
  GtkWindow *window = NULL;

  g_return_if_fail(IS_APPLICATION(self));

  window = application_window(self);
  if (window)
    sidebar_window_append_content(window, child);
}
