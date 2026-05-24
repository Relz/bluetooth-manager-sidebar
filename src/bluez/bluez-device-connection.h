#pragma once

#include <glib.h>

#include "bluez/bluez-adapter.h"
#include "bluez/bluez-device-power.h"
#include "bluez/bluez-operation-reporter.h"
#include "bluez/bluez-state.h"

typedef struct _BluezDeviceConnectionController BluezDeviceConnectionController;

BluezDeviceConnectionController *bluez_device_connection_controller_new(BluezState *state,
                                                                               BluezOperationReporter *reporter,
                                                                               BluezAdapterController *adapter);
void bluez_device_connection_controller_free(BluezDeviceConnectionController *controller);
void bluez_device_connection_controller_shutdown(BluezDeviceConnectionController *controller);
gboolean bluez_device_connection_controller_is_connecting(BluezDeviceConnectionController *controller,
                                                              const char *device_path);
void bluez_device_connection_controller_connect(BluezDeviceConnectionController *controller,
                                                     const char *device_path);
void bluez_device_connection_controller_disconnect(BluezDeviceConnectionController *controller,
                                                        const char *device_path);
BluezDevicePowerGate *bluez_device_connection_controller_power_gate(BluezDeviceConnectionController *controller);
