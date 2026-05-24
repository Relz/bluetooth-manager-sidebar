#pragma once

#include "bluez/bluez-device-property-operation.h"
#include "bluez/bluez-operation-reporter.h"

void bluez_device_property_report_finished(BluezOperationReporter *reporter,
                                           BluezDevicePropertyOperation operation,
                                           const char *device_path,
                                           gboolean value,
                                           GVariant *result_value,
                                           gboolean success,
                                           const GError *error);
void bluez_device_property_report_unavailable(BluezOperationReporter *reporter,
                                              BluezDevicePropertyOperation operation,
                                              const char *device_path,
                                              GVariant *result_value);
