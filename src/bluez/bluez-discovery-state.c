#include "bluez/bluez-discovery-state.h"

const char *
bluez_discovery_state_primary_adapter(BluezState *state)
{
  return state ? bluez_state_primary_adapter(state) : NULL;
}

gboolean
bluez_discovery_state_adapter_is_discovering(BluezState *state,
                                             const char *adapter_path)
{
  BluezProperties *adapter = NULL;

  if (!state || !adapter_path)
    return FALSE;

  adapter = bluez_object_cache_adapter(bluez_state_cache(state), adapter_path);
  return bluez_properties_get_bool(adapter, "Discovering", FALSE);
}

gboolean
bluez_discovery_state_adapter_is_powered(BluezState *state,
                                         const char *adapter_path)
{
  BluezProperties *adapter = NULL;

  if (!state || !adapter_path)
    return FALSE;

  adapter = bluez_object_cache_adapter(bluez_state_cache(state), adapter_path);
  return bluez_properties_get_bool(adapter, "Powered", FALSE);
}

gboolean
bluez_discovery_state_has_powered_adapter(BluezState *state,
                                          const char **out_adapter_path)
{
  const char *adapter_path = bluez_discovery_state_primary_adapter(state);

  if (out_adapter_path)
    *out_adapter_path = adapter_path;

  return state && bluez_state_connection(state) && adapter_path &&
    bluez_discovery_state_adapter_is_powered(state, adapter_path);
}
