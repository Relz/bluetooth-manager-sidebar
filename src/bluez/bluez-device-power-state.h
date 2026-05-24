#pragma once

#include "bluez/bluez-device-power.h"

typedef struct {
  char *adapter_path;
  BluezPowerGateStatus failure_status;
  gboolean available;
  gboolean powered;
} BluezDevicePowerState;

BluezDevicePowerState bluez_device_power_state_resolve(BluezState *state,
                                                          const char *device_path);
gboolean bluez_device_power_state_adapter_is_powered(BluezState *state,
                                                     const char *adapter_path);
void bluez_device_power_state_clear(BluezDevicePowerState *power_state);
