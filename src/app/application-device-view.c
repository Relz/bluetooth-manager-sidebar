#include "app/application-device-view.h"

#include <gtk/gtk.h>

#include "core/device-icon-constants.h"

const char *
application_device_view_icon_name(BluezProperties *device)
{
  static const struct {
    const char *bluez_name;
    const char *icon_name;
  } icon_names[] = {
    { "audio-headset", "audio-headphones-symbolic" },
    { "headset", "audio-headphones-symbolic" },
    { "headphones", "audio-headphones-symbolic" },
    { "audio-card", "audio-speakers-symbolic" },
    { "speaker", "audio-speakers-symbolic" },
    { "input-keyboard", "input-keyboard-symbolic" },
    { "input-mouse", "input-mouse-symbolic" },
    { "phone", "phone-symbolic" },
    { "computer", "computer-symbolic" },
  };
  g_autofree char *icon = bluez_properties_dup_string(device, "Icon");
  GdkDisplay *display;
  GtkIconTheme *theme;

  if (icon && *icon) {
    for (guint i = 0; i < G_N_ELEMENTS(icon_names); i++) {
      if (g_str_equal(icon, icon_names[i].bluez_name))
        return icon_names[i].icon_name;
    }
  }

  display = gdk_display_get_default();
  theme = display ? gtk_icon_theme_get_for_display(display) : NULL;
  if (theme && gtk_icon_theme_has_icon(theme, DEFAULT_DEVICE_ICON))
    return DEFAULT_DEVICE_ICON;

  return FALLBACK_DEVICE_ICON;
}
