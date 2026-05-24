#include "app/application-bluetooth-commands.h"

#include "app/application-data-access.h"
#include "app/application-bluez-service-access.h"
#include "bluez/bluez-client-adapter.h"
#include "bluez/bluez-client-device.h"
#include "bluez/bluez-client-discovery.h"

void
application_commands_set_adapter_power(Application *self, gboolean powered)
{
  BluezClient *bluez = application_services_bluez_client(self);

  if (bluez)
    bluez_client_set_adapter_power(bluez, powered);
}

void
application_commands_set_adapter_discoverable(Application *self, gboolean discoverable)
{
  BluezClient *bluez = application_services_bluez_client(self);

  if (bluez)
    bluez_client_set_adapter_discoverable(bluez, discoverable);
}

void
application_commands_toggle_discovery(Application *self)
{
  BluezClient *bluez = application_services_bluez_client(self);

  if (bluez)
    bluez_client_toggle_discovery(bluez);
}

void
application_commands_activate_device(Application *self, const char *device_path)
{
  BluezClient *bluez = application_services_bluez_client(self);

  if (bluez && device_path)
    bluez_client_activate_device(bluez, device_path);
}

void
application_commands_trust_device(Application *self, const char *device_path)
{
  BluezClient *bluez = application_services_bluez_client(self);

  if (bluez && device_path)
    bluez_client_trust_device(bluez, device_path);
}

void
application_commands_set_device_alias(Application *self,
                                      const char *device_path,
                                      const char *alias)
{
  BluezClient *bluez = application_services_bluez_client(self);
  BluezProperties *device = NULL;
  g_autofree char *current_alias = NULL;
  g_autofree char *stripped = g_strdup(alias ? alias : "");

  if (!device_path || !bluez)
    return;

  g_strstrip(stripped);
  device = application_device_properties(self, device_path);
  current_alias = bluez_properties_dup_string(device, "Alias");
  if (g_strcmp0(stripped, current_alias ? current_alias : "") != 0)
    bluez_client_set_device_alias(bluez, device_path, stripped);
}

void
application_commands_set_device_trusted(Application *self,
                                        const char *device_path,
                                        gboolean trusted)
{
  BluezClient *bluez = application_services_bluez_client(self);

  if (bluez && device_path)
    bluez_client_set_device_trusted(bluez, device_path, trusted);
}

void
application_commands_set_device_blocked(Application *self,
                                        const char *device_path,
                                        gboolean blocked)
{
  BluezClient *bluez = application_services_bluez_client(self);

  if (bluez && device_path)
    bluez_client_set_device_blocked(bluez, device_path, blocked);
}

void
application_commands_forget_device(Application *self, const char *device_path)
{
  BluezClient *bluez = application_services_bluez_client(self);

  if (bluez && device_path)
    bluez_client_forget_device(bluez, device_path);
}
