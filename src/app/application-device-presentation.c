#include "app/application-device-presentation.h"

#include "app/application-data-access.h"
#include "app/application-bluez-service-access.h"
#include "bluez/bluez-client-query.h"
#include "bluez/bluez-device-display.h"
#include "core/display.h"

static gboolean
string_has_text(const char *value)
{
  return value && *value;
}

char *
application_device_presentation_dup_display_name(Application *self, const char *device_path)
{
  return bluez_object_cache_dup_device_display_name(application_bluez_cache(self),
                                                   device_path,
                                                   "Unknown device");
}

static void
application_append_subtitle_part(GString *subtitle, const char *part)
{
  if (!string_has_text(part))
    return;

  if (subtitle->len > 0)
    g_string_append(subtitle, " | ");
  g_string_append(subtitle, part);
}

char *
application_device_presentation_dup_subtitle(Application *self, const char *device_path)
{
  BluezProperties *device = application_device_properties(self, device_path);
  GString *subtitle = g_string_new(NULL);
  g_autofree char *address = bluez_properties_dup_string(device, "Address");
  g_autofree char *safe_address = display_dup_safe_text(address, NULL);
  gboolean found = FALSE;
  gint rssi;
  gboolean battery_found = FALSE;
  int battery;

  if (string_has_text(safe_address))
    application_append_subtitle_part(subtitle, safe_address);

  gboolean trusted = FALSE;
  if (bluez_properties_lookup_bool(device, "Trusted", &trusted)) {
    g_autofree char *part = g_strdup_printf("Trusted: %s", trusted ? "Yes" : "No");
    application_append_subtitle_part(subtitle, part);
  }

  gboolean blocked = FALSE;
  if (bluez_properties_lookup_bool(device, "Blocked", &blocked)) {
    g_autofree char *part = g_strdup_printf("Blocked: %s", blocked ? "Yes" : "No");
    application_append_subtitle_part(subtitle, part);
  }

  rssi = bluez_properties_get_int(device, "RSSI", &found);
  if (found) {
    g_autofree char *part = g_strdup_printf("RSSI: %d", rssi);
    application_append_subtitle_part(subtitle, part);
  }

  BluezClient *bluez = application_services_bluez_client(self);
  battery = bluez ? bluez_client_device_battery(bluez, device_path, &battery_found) : 0;
  if (battery_found) {
    g_autofree char *part = g_strdup_printf("Battery: %d%%", battery);
    application_append_subtitle_part(subtitle, part);
  }

  if (subtitle->len == 0)
    g_string_append(subtitle, "No device details available");

  return g_string_free(subtitle, FALSE);
}
