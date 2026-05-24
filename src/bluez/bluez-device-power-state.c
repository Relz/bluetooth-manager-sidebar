#include "bluez/bluez-device-power-state.h"

#include "bluez/bluez-cache-query.h"
#include "bluez/bluez-properties.h"

gboolean
bluez_device_power_state_adapter_is_powered(BluezState *state,
                                            const char *adapter_path)
{
  BluezProperties *adapter = NULL;

  if (!state || !adapter_path)
    return FALSE;

  adapter = bluez_object_cache_adapter(bluez_state_cache(state), adapter_path);
  return bluez_properties_get_bool(adapter, "Powered", FALSE);
}

BluezDevicePowerState
bluez_device_power_state_resolve(BluezState *state, const char *device_path)
{
  BluezDevicePowerState power_state = {
    .failure_status = BLUEZ_POWER_GATE_STATUS_READY,
  };

  if (!state || !device_path || !bluez_state_connection(state)) {
    power_state.failure_status = BLUEZ_POWER_GATE_STATUS_SYSTEM_BUS_UNAVAILABLE;
    return power_state;
  }

  power_state.adapter_path = bluez_cache_query_dup_device_adapter_path(bluez_state_cache(state),
                                                                       device_path);
  if (!power_state.adapter_path) {
    power_state.failure_status = BLUEZ_POWER_GATE_STATUS_ADAPTER_UNAVAILABLE;
    return power_state;
  }

  power_state.available = TRUE;
  power_state.powered = bluez_device_power_state_adapter_is_powered(state,
                                                                   power_state.adapter_path);
  return power_state;
}

void
bluez_device_power_state_clear(BluezDevicePowerState *power_state)
{
  if (!power_state)
    return;

  g_clear_pointer(&power_state->adapter_path, g_free);
  power_state->failure_status = BLUEZ_POWER_GATE_STATUS_READY;
  power_state->available = FALSE;
  power_state->powered = FALSE;
}
