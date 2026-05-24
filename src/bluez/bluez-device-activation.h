#pragma once

#include <glib.h>

#include "bluez/bluez-properties.h"
#include "bluez/bluez-state.h"

typedef void (*BluezDeviceActivationCallback)(const char *device_path, gpointer user_data);

typedef struct {
  BluezDeviceActivationCallback connect;
  BluezDeviceActivationCallback disconnect;
  BluezDeviceActivationCallback pair_trust_and_connect;
  gpointer user_data;
} BluezDeviceActivationActions;

void bluez_device_activation_activate(BluezState *state,
                                          const char *device_path,
                                          const BluezDeviceActivationActions *actions);
void bluez_device_activation_activate_with_properties(BluezProperties *device,
                                                          const char *device_path,
                                                          const BluezDeviceActivationActions *actions);
