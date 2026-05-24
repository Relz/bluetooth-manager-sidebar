#pragma once

#include <glib.h>

#include "bluez/bluez-adapter.h"
#include "bluez/bluez-device-power.h"
#include "bluez/bluez-operation-reporter.h"
#include "bluez/bluez-state.h"

typedef struct _BluezDeviceConnectionPower BluezDeviceConnectionPower;

typedef void (*BluezDeviceConnectionResume)(const char *device_path, gpointer user_data);
typedef void (*BluezDeviceConnectionPendingChanged)(const char *device_path, gpointer user_data);

BluezDeviceConnectionPower *bluez_device_connection_power_new(BluezState *state,
                                                                 BluezOperationReporter *reporter,
                                                                 BluezAdapterController *adapter);
void bluez_device_connection_power_free(BluezDeviceConnectionPower *power);
void bluez_device_connection_power_clear(BluezDeviceConnectionPower *power);
BluezDevicePowerGate *bluez_device_connection_power_gate(BluezDeviceConnectionPower *power);

gboolean bluez_device_connection_power_request_connect(BluezDeviceConnectionPower *power,
                                                       const char *device_path,
                                                       BluezDeviceConnectionResume resume,
                                                       gpointer resume_data,
                                                       GDestroyNotify resume_destroy,
                                                       BluezDeviceConnectionPendingChanged pending_started,
                                                       BluezDeviceConnectionPendingChanged pending_finished,
                                                       gpointer pending_user_data);
