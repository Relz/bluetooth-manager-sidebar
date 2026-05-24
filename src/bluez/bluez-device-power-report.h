#pragma once

#include "bluez/bluez-device-power.h"
#include "bluez/bluez-operation-reporter.h"

gboolean bluez_device_power_gate_report_failure(BluezOperationReporter *reporter,
                                                const char *operation,
                                                const char *action,
                                                const char *device_path,
                                                BluezPowerGateStatus status);
