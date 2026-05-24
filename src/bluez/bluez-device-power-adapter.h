#pragma once

#include "bluez/bluez-adapter-power-workflow.h"

gboolean bluez_device_power_adapter_request_on(BluezState *state,
                                               BluezOperationReporter *reporter,
                                               BluezAdapterController *adapter,
                                               const char *adapter_path,
                                               BluezAdapterPowerFinished finished,
                                               gpointer user_data,
                                               GDestroyNotify destroy);
