#pragma once

#include "sidebar/sidebar-window-state.h"

void sidebar_window_chrome_proxy_set_title(SidebarWindowState *state, const char *title);
void sidebar_window_chrome_proxy_set_overview_mode(SidebarWindowState *state, gboolean overview);
void sidebar_window_chrome_proxy_set_power_state(SidebarWindowState *state,
                                                gboolean visible,
                                                gboolean sensitive,
                                                gboolean active);
