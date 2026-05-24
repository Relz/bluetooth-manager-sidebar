#pragma once

#include "sidebar/sidebar-window.h"
#include "sidebar/sidebar-window-state.h"

GtkWidget *sidebar_window_panel_build(const SidebarWindowCallbacks *callbacks,
                                      SidebarWindowState *state);
void sidebar_window_panel_clear_content(SidebarWindowState *state);
void sidebar_window_panel_append_content(SidebarWindowState *state, GtkWidget *child);
double sidebar_window_panel_scroll_value(SidebarWindowState *state);
void sidebar_window_panel_restore_scroll(SidebarWindowState *state, double value);
