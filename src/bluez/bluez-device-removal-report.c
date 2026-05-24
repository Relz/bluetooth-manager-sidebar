#include "bluez/bluez-device-removal-report.h"

#include "bluez/bluez-operation-ids.h"
#include "bluez/bluez-operation-messages.h"

void
bluez_device_removal_report_removing(BluezOperationReporter *reporter)
{
  bluez_operation_reporter_status(reporter, bluez_operation_message_removing_device());
}

void
bluez_device_removal_report_adapter_unavailable(BluezOperationReporter *reporter,
                                                const char *device_path)
{
  g_autofree char *message = bluez_operation_message_device_adapter_unavailable("remove");

  bluez_operation_reporter_device_result(reporter,
                                         BLUEZ_OPERATION_REMOVE_DEVICE,
                                         FALSE,
                                         message,
                                         device_path);
}

void
bluez_device_removal_report_finished(BluezOperationReporter *reporter,
                                     const char *device_path,
                                     gboolean success,
                                     const GError *error)
{
  if (success) {
    bluez_operation_reporter_device_result(reporter,
                                           BLUEZ_OPERATION_REMOVE_DEVICE,
                                           TRUE,
                                           bluez_operation_message_device_removed(),
                                           device_path);
    return;
  }

  g_autofree char *message = bluez_operation_message_remove_failed(error ? error->message : NULL);

  bluez_operation_reporter_device_result(reporter,
                                         BLUEZ_OPERATION_REMOVE_DEVICE,
                                         FALSE,
                                         message,
                                         device_path);
}
