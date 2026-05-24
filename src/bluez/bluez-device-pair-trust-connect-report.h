#pragma once

#include <gio/gio.h>

#include "bluez/bluez-device-power.h"
#include "bluez/bluez-operation-reporter.h"

void bluez_pair_trust_connect_report_pairing(BluezOperationReporter *reporter);
void bluez_pair_trust_connect_report_turning_power_on(BluezOperationReporter *reporter);
void bluez_pair_trust_connect_report_pair_success(BluezOperationReporter *reporter,
                                                  const char *device_path);
void bluez_pair_trust_connect_report_pair_failure(BluezOperationReporter *reporter,
                                                  const char *device_path,
                                                  const GError *error);
void bluez_pair_trust_connect_report_system_bus_unavailable(BluezOperationReporter *reporter,
                                                            const char *device_path);
void bluez_pair_trust_connect_report_power_failure(BluezOperationReporter *reporter,
                                                   const char *device_path,
                                                   BluezPowerGateStatus status);
