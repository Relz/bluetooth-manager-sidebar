#include "sidebar/sidebar-window.h"

#include "sidebar/sidebar-window-chrome-proxy.h"
#include "sidebar/sidebar-window-panel.h"
#include "sidebar/sidebar-window-shell.h"
#include "sidebar/sidebar-window-state.h"
#include "sidebar/sidebar-window-toast.h"

gboolean
sidebar_window_is_supported(void)
{
  return sidebar_window_shell_is_supported();
}

GtkWindow *
sidebar_window_new(GtkApplication *application,
                   const SidebarWindowCallbacks *callbacks,
                   const char *target_output_name)
{
  GtkWindow *window;
  GtkWidget *content;
  GtkEventController *key_controller;
  SidebarWindowState *state;

  g_return_val_if_fail(GTK_IS_APPLICATION(application), NULL);

  if (!sidebar_window_is_supported())
    return NULL;

  state = sidebar_window_state_new();
  window = sidebar_window_shell_create_application_window(application);
  sidebar_window_state_attach(window, state);
  sidebar_window_shell_initialize(window);
  sidebar_window_shell_configure(window, target_output_name);

  content = sidebar_window_panel_build(callbacks, state);
  adw_application_window_set_content(ADW_APPLICATION_WINDOW(window), content);

  key_controller = gtk_event_controller_key_new();
  if (callbacks && callbacks->key_pressed)
    g_signal_connect(key_controller, "key-pressed", callbacks->key_pressed, callbacks->user_data);
  gtk_widget_add_controller(GTK_WIDGET(window), key_controller);

  if (callbacks && callbacks->close_request)
    g_signal_connect(window, "close-request", callbacks->close_request, callbacks->user_data);

  return window;
}

void
sidebar_window_set_target_output(GtkWindow *window, const char *target_output_name)
{
  g_return_if_fail(GTK_IS_WINDOW(window));

  sidebar_window_shell_configure(window, target_output_name);
}

void
sidebar_window_clear_content(GtkWindow *window)
{
  sidebar_window_panel_clear_content(sidebar_window_state_from_window(window));
}

void
sidebar_window_append_content(GtkWindow *window, GtkWidget *child)
{
  sidebar_window_panel_append_content(sidebar_window_state_from_window(window), child);
}

double
sidebar_window_scroll_value(GtkWindow *window)
{
  return sidebar_window_panel_scroll_value(sidebar_window_state_from_window(window));
}

void
sidebar_window_restore_scroll(GtkWindow *window, double value)
{
  sidebar_window_panel_restore_scroll(sidebar_window_state_from_window(window), value);
}

void
sidebar_window_set_title(GtkWindow *window, const char *title)
{
  sidebar_window_chrome_proxy_set_title(sidebar_window_state_from_window(window), title);
}

void
sidebar_window_set_overview_mode(GtkWindow *window, gboolean overview)
{
  sidebar_window_chrome_proxy_set_overview_mode(sidebar_window_state_from_window(window), overview);
}

void
sidebar_window_set_power_state(GtkWindow *window,
                               gboolean visible,
                               gboolean sensitive,
                               gboolean active)
{
  sidebar_window_chrome_proxy_set_power_state(sidebar_window_state_from_window(window),
                                             visible,
                                             sensitive,
                                             active);
}

void
sidebar_window_show_toast(GtkWindow *window, const char *message)
{
  sidebar_window_toast_show(sidebar_window_state_from_window(window), message);
}
