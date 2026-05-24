#include "sidebar/sidebar-window-state.h"

#define SIDEBAR_WINDOW_STATE_KEY "bm-sidebar-window-state"

static void
sidebar_window_state_free(SidebarWindowState *state)
{
  if (!state)
    return;

  if (state->scroll_restore_source)
    g_source_remove(state->scroll_restore_source);
  sidebar_chrome_free(state->chrome);
  g_free(state);
}

SidebarWindowState *
sidebar_window_state_new(void)
{
  return g_new0(SidebarWindowState, 1);
}

void
sidebar_window_state_attach(GtkWindow *window, SidebarWindowState *state)
{
  g_object_set_data_full(G_OBJECT(window),
                         SIDEBAR_WINDOW_STATE_KEY,
                         state,
                         (GDestroyNotify)sidebar_window_state_free);
}

SidebarWindowState *
sidebar_window_state_from_window(GtkWindow *window)
{
  return window ? g_object_get_data(G_OBJECT(window), SIDEBAR_WINDOW_STATE_KEY) : NULL;
}
