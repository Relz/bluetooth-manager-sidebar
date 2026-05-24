#include "sidebar/sidebar-window-chrome-proxy.h"

void
sidebar_window_chrome_proxy_set_title(SidebarWindowState *state, const char *title)
{
  if (state)
    sidebar_chrome_set_title(state->chrome, title);
}

void
sidebar_window_chrome_proxy_set_overview_mode(SidebarWindowState *state, gboolean overview)
{
  if (state)
    sidebar_chrome_set_overview_mode(state->chrome, overview);
}

void
sidebar_window_chrome_proxy_set_power_state(SidebarWindowState *state,
                                            gboolean visible,
                                            gboolean sensitive,
                                            gboolean active)
{
  if (state)
    sidebar_chrome_set_power_state(state->chrome, visible, sensitive, active);
}
