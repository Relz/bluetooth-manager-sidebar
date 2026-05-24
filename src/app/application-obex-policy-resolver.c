#include "app/application-obex-policy-resolver.h"

#include "app/application-data-access.h"
#include "app/application-bluez-service-access.h"
#include "app/application-settings-service-access.h"
#include "bluez/bluez-client-query.h"
#include "bluez/bluez-device-display.h"

char *
application_obex_policy_resolver_dup_received_directory(Application *self)
{
  const char *location = sidebar_settings_get_received_files_location(application_services_settings(self));

  return g_strdup(location ? location : "");
}

gboolean
application_obex_policy_resolver_auto_accept_trusted_files(Application *self)
{
  return sidebar_settings_get_auto_accept_trusted_files(application_services_settings(self));
}

char *
application_obex_policy_resolver_dup_device_address(Application *self, const char *device_path)
{
  BluezProperties *device = application_device_properties(self, device_path);

  return bluez_properties_dup_string(device, "Address");
}

gboolean
application_obex_policy_resolver_is_trusted_address(Application *self, const char *address)
{
  BluezClient *bluez = application_services_bluez_client(self);
  g_autofree char *normalized = address ? g_utf8_casefold(address, -1) : NULL;
  g_auto(GStrv) paths = bluez ? bluez_client_dup_adapter_device_paths(bluez, NULL) : NULL;

  if (!normalized)
    return FALSE;

  for (guint i = 0; paths && paths[i]; i++) {
    BluezProperties *device = application_device_properties(self, paths[i]);
    g_autofree char *device_address = bluez_properties_dup_string(device, "Address");
    g_autofree char *device_normalized = device_address ? g_utf8_casefold(device_address, -1) : NULL;

    if (device_normalized && g_str_equal(device_normalized, normalized))
      return bluez_properties_get_bool(device, "Trusted", FALSE);
  }

  return FALSE;
}

char *
application_obex_policy_resolver_dup_device_name_for_address(Application *self,
                                                             const char *address,
                                                             const char *fallback)
{
  BluezObjectCache *cache = application_bluez_cache(self);

  return bluez_object_cache_dup_device_display_name_for_address(cache, address, fallback);
}
