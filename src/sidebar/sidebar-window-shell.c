#include "sidebar/sidebar-window-shell.h"

#include <gtk4-layer-shell.h>

#include "app/app-metadata.h"

#define LAYER_SHELL_ON_DEMAND_PROTOCOL_VERSION 4

static GtkLayerShellKeyboardMode keyboard_mode(void);
static void set_respect_close_if_supported(GtkWindow *window);
static GdkMonitor *target_monitor(const char *target_output_name);

gboolean
sidebar_window_shell_is_supported(void)
{
  return gtk_layer_is_supported();
}

GtkWindow *
sidebar_window_shell_create_application_window(GtkApplication *application)
{
  GtkWindow *window = GTK_WINDOW(adw_application_window_new(application));

  gtk_window_set_title(window, APP_NAME);
  gtk_window_set_default_size(window, 1, 1);
  gtk_widget_set_size_request(GTK_WIDGET(window), 1, 1);
  gtk_window_set_decorated(window, FALSE);
  gtk_widget_add_css_class(GTK_WIDGET(window), "bm-sidebar-window");
  return window;
}

void
sidebar_window_shell_initialize(GtkWindow *window)
{
  gtk_layer_init_for_window(window);
  gtk_layer_set_namespace(window, COMMAND_NAME);
}

void
sidebar_window_shell_configure(GtkWindow *window, const char *target_output_name)
{
  g_autoptr(GdkMonitor) monitor = NULL;

  monitor = target_monitor(target_output_name);
  if (monitor)
    gtk_layer_set_monitor(window, monitor);
  gtk_layer_set_layer(window, GTK_LAYER_SHELL_LAYER_TOP);
  gtk_layer_set_exclusive_zone(window, 0);
  gtk_layer_set_keyboard_mode(window, keyboard_mode());
  set_respect_close_if_supported(window);
  gtk_layer_set_anchor(window, GTK_LAYER_SHELL_EDGE_LEFT, TRUE);
  gtk_layer_set_anchor(window, GTK_LAYER_SHELL_EDGE_RIGHT, TRUE);
  gtk_layer_set_anchor(window, GTK_LAYER_SHELL_EDGE_TOP, TRUE);
  gtk_layer_set_anchor(window, GTK_LAYER_SHELL_EDGE_BOTTOM, TRUE);
}

static GtkLayerShellKeyboardMode
keyboard_mode(void)
{
  if (gtk_layer_get_protocol_version() >= LAYER_SHELL_ON_DEMAND_PROTOCOL_VERSION)
    return (GtkLayerShellKeyboardMode)2;
  return GTK_LAYER_SHELL_KEYBOARD_MODE_EXCLUSIVE;
}

static void
set_respect_close_if_supported(GtkWindow *window)
{
  if (gtk_layer_get_major_version() > 1 ||
      (gtk_layer_get_major_version() == 1 && gtk_layer_get_minor_version() >= 3))
    gtk_layer_set_respect_close(window, TRUE);
}

static GdkMonitor *
monitor_for_output_name(GListModel *monitors, const char *output_name)
{
  guint count;

  if (!output_name)
    return NULL;

  count = g_list_model_get_n_items(monitors);
  for (guint i = 0; i < count; i++) {
    g_autoptr(GdkMonitor) monitor = g_list_model_get_item(monitors, i);
    if (g_strcmp0(gdk_monitor_get_connector(monitor), output_name) == 0)
      return g_object_ref(monitor);
  }

  return NULL;
}

static GdkMonitor *
target_monitor(const char *target_output_name)
{
  GdkDisplay *display = gdk_display_get_default();
  GListModel *monitors;

  if (!display)
    return NULL;

  monitors = gdk_display_get_monitors(display);
  if (target_output_name) {
    GdkMonitor *monitor = monitor_for_output_name(monitors, target_output_name);
    if (monitor)
      return monitor;
  }

  if (g_list_model_get_n_items(monitors) < 1)
    return NULL;
  return g_list_model_get_item(monitors, 0);
}
