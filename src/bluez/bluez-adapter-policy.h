#pragma once

#include "bluez/bluez-state.h"

typedef enum {
  BLUEZ_ADAPTER_POLICY_READY,
  BLUEZ_ADAPTER_POLICY_NO_ADAPTER,
  BLUEZ_ADAPTER_POLICY_DISCOVERABLE_REQUIRES_POWER,
} BluezAdapterPolicyResult;

BluezAdapterPolicyResult bluez_adapter_policy_can_set_power(BluezState *state,
                                                               const char *adapter_path);
BluezAdapterPolicyResult bluez_adapter_policy_can_set_discoverable(BluezState *state,
                                                                      const char *adapter_path,
                                                                      gboolean discoverable);
