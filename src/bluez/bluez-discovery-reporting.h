#pragma once

#include <gio/gio.h>

#include "bluez/bluez-operation-reporter.h"

void bluez_discovery_reporting_start_finished(BluezOperationReporter *reporter,
                                              gboolean success,
                                              const GError *error);
void bluez_discovery_reporting_stop_finished(BluezOperationReporter *reporter,
                                              gboolean success,
                                              const GError *error,
                                              gboolean report_stopped_message);
void bluez_discovery_reporting_requires_power(BluezOperationReporter *reporter);
void bluez_discovery_reporting_turning_power_on(BluezOperationReporter *reporter);
void bluez_discovery_reporting_power_on_pending(BluezOperationReporter *reporter);
