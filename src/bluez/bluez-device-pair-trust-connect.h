#pragma once

#include <glib.h>

#include "bluez/bluez-device-power.h"
#include "bluez/bluez-operation-reporter.h"
#include "bluez/bluez-state.h"

typedef struct _BluezDevicePairTrustConnectWorkflow BluezDevicePairTrustConnectWorkflow;
typedef struct _BluezDevicePropertiesController BluezDevicePropertiesController;

typedef void (*BluezDeviceConnect)(const char *device_path, gpointer user_data);

BluezDevicePairTrustConnectWorkflow *bluez_device_pair_trust_connect_workflow_new(
  BluezState *state,
  BluezOperationReporter *reporter,
  BluezDevicePowerGate *power_gate,
  BluezDevicePropertiesController *properties,
  BluezDeviceConnect connect_device,
  gpointer connect_user_data,
  GDestroyNotify connect_destroy);
void bluez_device_pair_trust_connect_workflow_free(BluezDevicePairTrustConnectWorkflow *workflow);
void bluez_device_pair_trust_connect_workflow_start(BluezDevicePairTrustConnectWorkflow *workflow,
                                                    const char *device_path);
