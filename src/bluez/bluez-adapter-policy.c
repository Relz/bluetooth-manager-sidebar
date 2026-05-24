#include "bluez/bluez-adapter-policy.h"

BluezAdapterPolicyResult
bluez_adapter_policy_can_set_power(BluezState *state, const char *adapter_path)
{
  if (!bluez_state_connection(state) || !adapter_path)
    return BLUEZ_ADAPTER_POLICY_NO_ADAPTER;

  return BLUEZ_ADAPTER_POLICY_READY;
}

BluezAdapterPolicyResult
bluez_adapter_policy_can_set_discoverable(BluezState *state,
                                          const char *adapter_path,
                                          gboolean discoverable)
{
  BluezProperties *adapter = NULL;

  if (!bluez_state_connection(state) || !adapter_path)
    return BLUEZ_ADAPTER_POLICY_NO_ADAPTER;

  if (!discoverable)
    return BLUEZ_ADAPTER_POLICY_READY;

  adapter = bluez_object_cache_adapter(bluez_state_cache(state), adapter_path);
  if (!bluez_properties_get_bool(adapter, "Powered", FALSE))
    return BLUEZ_ADAPTER_POLICY_DISCOVERABLE_REQUIRES_POWER;

  return BLUEZ_ADAPTER_POLICY_READY;
}
