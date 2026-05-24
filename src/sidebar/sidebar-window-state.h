#pragma once

#include <adwaita.h>

#include "sidebar/sidebar-chrome.h"

typedef struct {
  AdwToastOverlay *toast_overlay;
  GtkScrolledWindow *scrolled;
  GtkBox *content_box;
  GtkWidget *scroll_fade_top;
  GtkWidget *scroll_fade_bottom;
  SidebarChrome *chrome;
  guint scroll_restore_source;
  guint scroll_restore_attempts;
  double pending_scroll_value;
  GCallback surface_released;
  gpointer surface_released_user_data;
  gboolean outside_click_started;
} SidebarWindowState;

SidebarWindowState *sidebar_window_state_new(void);
void sidebar_window_state_attach(GtkWindow *window, SidebarWindowState *state);
SidebarWindowState *sidebar_window_state_from_window(GtkWindow *window);
