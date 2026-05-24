#include "obex/obex-notifications.h"

#include "core/device-icon-constants.h"
#include "core/display.h"

static char *
obex_notification_format_size(guint64 size)
{
  double amount = size;
  static const char *units[] = { "B", "KB", "MB", "GB" };

  for (guint i = 0; i < G_N_ELEMENTS(units); i++) {
    if (amount < 1024.0 || i == G_N_ELEMENTS(units) - 1) {
      if (i == 0)
        return g_strdup_printf("%" G_GUINT64_FORMAT " %s", size, units[i]);
      return g_strdup_printf("%.1f %s", amount, units[i]);
    }
    amount /= 1024.0;
  }

  return NULL;
}

GNotification *
obex_notification_new_authorization(const char *device_name,
                                        const char *file_name,
                                        gboolean has_size,
                                        guint64 size,
                                        const char *received_dir,
                                        const char *transfer_path)
{
  g_autofree char *formatted_size = has_size ? obex_notification_format_size(size) : NULL;
  g_autofree char *safe_device_name = display_dup_safe_text(device_name, "Bluetooth device");
  g_autofree char *safe_file_name = display_dup_safe_text(file_name, "Bluetooth file");
  g_autofree char *safe_destination = display_dup_safe_text(received_dir, "configured folder");
  g_autofree char *body = NULL;
  g_autoptr(GIcon) icon = NULL;
  GNotification *notification = NULL;
  const char *name = safe_device_name ? safe_device_name : "Bluetooth device";
  const char *display_file_name = safe_file_name ? safe_file_name : "Bluetooth file";
  const char *destination = safe_destination ? safe_destination : "configured folder";
  const char *path = transfer_path ? transfer_path : "";

  if (formatted_size)
    body = g_strdup_printf("%s wants to send:\n\n%s\n%s\n\nSave to:\n%s",
                           name,
                           display_file_name,
                           formatted_size,
                           destination);
  else
    body = g_strdup_printf("%s wants to send:\n\n%s\n\nSave to:\n%s",
                           name,
                           display_file_name,
                           destination);

  notification = g_notification_new("Receive Bluetooth file?");
  g_notification_set_body(notification, body);
  icon = g_themed_icon_new(DEFAULT_DEVICE_ICON);
  g_notification_set_icon(notification, icon);
  g_notification_set_priority(notification, G_NOTIFICATION_PRIORITY_HIGH);
  g_notification_add_button_with_target_value(notification,
                                             "Receive",
                                             "app.receive-file",
                                             g_variant_new_string(path));
  g_notification_add_button_with_target_value(notification,
                                             "Reject",
                                             "app.reject-file",
                                             g_variant_new_string(path));
  return notification;
}

char *
obex_notification_dup_authorization_toast(const char *device_name)
{
  g_autofree char *safe_device_name = display_dup_safe_text(device_name, "Bluetooth device");

  return g_strdup_printf("%s wants to send a file", safe_device_name ? safe_device_name : "Bluetooth device");
}

GNotification *
obex_notification_new_completed(const char *final_path)
{
  g_autoptr(GIcon) icon = NULL;
  g_autofree char *basename = g_path_get_basename(final_path);
  g_autofree char *dirname = g_path_get_dirname(final_path);
  g_autofree char *safe_basename = display_dup_safe_text(basename, "Bluetooth file");
  g_autofree char *safe_dirname = display_dup_safe_text(dirname, "destination folder");
  g_autofree char *body = g_strdup_printf("%s\nSaved to %s",
                                          safe_basename ? safe_basename : "Bluetooth file",
                                          safe_dirname ? safe_dirname : "destination folder");
  GNotification *notification = NULL;

  notification = g_notification_new("Bluetooth file received");
  g_notification_set_body(notification, body);
  icon = g_themed_icon_new(DEFAULT_DEVICE_ICON);
  g_notification_set_icon(notification, icon);
  return notification;
}

GNotification *
obex_notification_new_outgoing_rejected(const char *device_name, const char *file_name)
{
  g_autoptr(GIcon) icon = NULL;
  g_autofree char *safe_device_name = display_dup_safe_text(device_name, "Bluetooth device");
  g_autofree char *safe_file_name = display_dup_safe_text(file_name, "Bluetooth file");
  g_autofree char *body = g_strdup_printf("%s did not accept %s",
                                          safe_device_name ? safe_device_name : "Bluetooth device",
                                          safe_file_name ? safe_file_name : "Bluetooth file");
  GNotification *notification = NULL;

  notification = g_notification_new("Bluetooth file not accepted");
  g_notification_set_body(notification, body);
  icon = g_themed_icon_new(DEFAULT_DEVICE_ICON);
  g_notification_set_icon(notification, icon);
  return notification;
}
