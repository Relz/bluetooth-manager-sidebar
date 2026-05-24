#pragma once

#include <adwaita.h>

#include "sidebar/sidebar-chrome.h"

typedef struct {
  gpointer user_data;
  GCallback surface_released;
  GCallback key_pressed;
  GCallback close_request;
  SidebarChromeCallbacks chrome;
} SidebarWindowCallbacks;

gboolean sidebar_window_is_supported(void);
GtkWindow *sidebar_window_new(GtkApplication *application,
                              const SidebarWindowCallbacks *callbacks,
                              const char *target_output_name);
void sidebar_window_set_target_output(GtkWindow *window, const char *target_output_name);
void sidebar_window_clear_content(GtkWindow *window);
void sidebar_window_append_content(GtkWindow *window, GtkWidget *child);
double sidebar_window_scroll_value(GtkWindow *window);
void sidebar_window_restore_scroll(GtkWindow *window, double value);
void sidebar_window_set_title(GtkWindow *window, const char *title);
void sidebar_window_set_overview_mode(GtkWindow *window, gboolean overview);
void sidebar_window_set_power_state(GtkWindow *window,
                                    gboolean visible,
                                    gboolean sensitive,
                                    gboolean active);
void sidebar_window_show_toast(GtkWindow *window, const char *message);
