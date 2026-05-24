#include "bluez/bluez-device-activation.h"

#include "bluez/bluez-cache.h"

void
bluez_device_activation_activate_with_properties(BluezProperties *device,
                                                     const char *device_path,
                                                     const BluezDeviceActivationActions *actions)
{
  if (!device_path || !actions)
    return;

  if (bluez_properties_get_bool(device, "Connected", FALSE)) {
    if (actions->disconnect)
      actions->disconnect(device_path, actions->user_data);
  } else if (bluez_properties_get_bool(device, "Paired", FALSE)) {
    if (actions->connect)
      actions->connect(device_path, actions->user_data);
  } else if (actions->pair_trust_and_connect) {
    actions->pair_trust_and_connect(device_path, actions->user_data);
  }
}

void
bluez_device_activation_activate(BluezState *state,
                                     const char *device_path,
                                     const BluezDeviceActivationActions *actions)
{
  BluezProperties *device = NULL;

  if (!state || !device_path)
    return;

  device = bluez_object_cache_device(bluez_state_cache(state), device_path);
  bluez_device_activation_activate_with_properties(device, device_path, actions);
}
