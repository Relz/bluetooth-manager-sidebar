#include "bluez/bluez-device-property-report.h"

#include "bluez/bluez-operation-messages.h"

static void
bluez_device_property_report(BluezOperationReporter *reporter,
                             BluezDevicePropertyOperation operation,
                             gboolean success,
                             const char *message,
                             const char *device_path,
                             GVariant *value)
{
  bluez_operation_reporter_result(reporter,
                                  bluez_device_property_operation_name(operation),
                                  success,
                                  message,
                                  device_path,
                                  NULL,
                                  value);
}

void
bluez_device_property_report_finished(BluezOperationReporter *reporter,
                                      BluezDevicePropertyOperation operation,
                                      const char *device_path,
                                      gboolean value,
                                      GVariant *result_value,
                                      gboolean success,
                                      const GError *error)
{
  if (success) {
    bluez_device_property_report(reporter,
                                 operation,
                                 TRUE,
                                 bluez_device_property_operation_success_message(operation, value),
                                 device_path,
                                 result_value);
    return;
  }

  g_autofree char *message = bluez_device_property_operation_failure_message(
    operation,
    error ? error->message : NULL);

  bluez_device_property_report(reporter,
                               operation,
                               FALSE,
                               message,
                               device_path,
                               result_value);
}

void
bluez_device_property_report_unavailable(BluezOperationReporter *reporter,
                                         BluezDevicePropertyOperation operation,
                                         const char *device_path,
                                         GVariant *result_value)
{
  bluez_device_property_report(reporter,
                               operation,
                               FALSE,
                               bluez_operation_message_system_bus_unavailable(),
                               device_path,
                               result_value);
}
