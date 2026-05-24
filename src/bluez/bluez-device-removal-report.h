#pragma once

#include "bluez/bluez-operation-reporter.h"

void bluez_device_removal_report_removing(BluezOperationReporter *reporter);
void bluez_device_removal_report_adapter_unavailable(BluezOperationReporter *reporter,
                                                     const char *device_path);
void bluez_device_removal_report_finished(BluezOperationReporter *reporter,
                                          const char *device_path,
                                          gboolean success,
                                          const GError *error);
