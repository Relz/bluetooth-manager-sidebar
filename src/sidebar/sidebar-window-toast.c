#include "sidebar/sidebar-window-toast.h"

#include "core/display.h"

void
sidebar_window_toast_show(SidebarWindowState *state, const char *message)
{
  g_autofree char *safe_message = display_dup_safe_text(message, NULL);

  if (state && state->toast_overlay && safe_message)
    adw_toast_overlay_add_toast(state->toast_overlay, adw_toast_new(safe_message));
}
