#include "bluez/bluez-device-connection-report.h"

#include "bluez/bluez-operation-ids.h"
#include "bluez/bluez-operation-messages.h"

static void
bluez_device_connection_report_result(BluezOperationReporter *reporter,
                                      const char *operation,
                                      gboolean success,
                                      const char *message,
                                      const char *device_path)
{
  if (!reporter)
    return;

  bluez_operation_reporter_device_result(reporter,
                                         operation,
                                         success,
                                         message,
                                         device_path);
}

void
bluez_device_connection_report_connecting(BluezOperationReporter *reporter)
{
  if (reporter)
    bluez_operation_reporter_status(reporter, bluez_operation_message_connecting_device());
}

void
bluez_device_connection_report_disconnecting(BluezOperationReporter *reporter)
{
  if (reporter)
    bluez_operation_reporter_status(reporter, bluez_operation_message_disconnecting_device());
}

void
bluez_device_connection_report_connected(BluezOperationReporter *reporter,
                                         const char *device_path)
{
  bluez_device_connection_report_result(reporter,
                                        BLUEZ_OPERATION_CONNECT_DEVICE,
                                        TRUE,
                                        bluez_operation_message_device_connected(),
                                        device_path);
}

void
bluez_device_connection_report_disconnected(BluezOperationReporter *reporter,
                                            const char *device_path)
{
  bluez_device_connection_report_result(reporter,
                                        BLUEZ_OPERATION_DISCONNECT_DEVICE,
                                        TRUE,
                                        bluez_operation_message_device_disconnected(),
                                        device_path);
}

void
bluez_device_connection_report_connect_failed(BluezOperationReporter *reporter,
                                              const char *device_path,
                                              const GError *error)
{
  g_autofree char *message = bluez_operation_message_connect_failed(error);

  bluez_device_connection_report_result(reporter,
                                        BLUEZ_OPERATION_CONNECT_DEVICE,
                                        FALSE,
                                        message,
                                        device_path);
}

void
bluez_device_connection_report_disconnect_failed(BluezOperationReporter *reporter,
                                                 const char *device_path,
                                                 const GError *error)
{
  g_autofree char *message = bluez_operation_message_disconnect_failed(error);

  bluez_device_connection_report_result(reporter,
                                        BLUEZ_OPERATION_DISCONNECT_DEVICE,
                                        FALSE,
                                        message,
                                        device_path);
}

void
bluez_device_connection_report_connect_unavailable(BluezOperationReporter *reporter,
                                                   const char *device_path)
{
  bluez_device_connection_report_result(reporter,
                                        BLUEZ_OPERATION_CONNECT_DEVICE,
                                        FALSE,
                                        bluez_operation_message_system_bus_unavailable(),
                                        device_path);
}

void
bluez_device_connection_report_disconnect_unavailable(BluezOperationReporter *reporter,
                                                      const char *device_path)
{
  bluez_device_connection_report_result(reporter,
                                        BLUEZ_OPERATION_DISCONNECT_DEVICE,
                                        FALSE,
                                        bluez_operation_message_system_bus_unavailable(),
                                        device_path);
}

void
bluez_device_connection_report_already_in_progress(BluezOperationReporter *reporter,
                                                   const char *device_path)
{
  bluez_device_connection_report_result(reporter,
                                        BLUEZ_OPERATION_CONNECT_DEVICE,
                                        FALSE,
                                        bluez_operation_message_connection_already_in_progress(),
                                        device_path);
}
