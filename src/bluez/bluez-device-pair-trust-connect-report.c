#include "bluez/bluez-device-pair-trust-connect-report.h"

#include "bluez/bluez-device-power-report.h"
#include "bluez/bluez-operation-ids.h"
#include "bluez/bluez-operation-messages.h"

static void
bluez_pair_trust_connect_report_result(BluezOperationReporter *reporter,
                                       gboolean success,
                                       const char *message,
                                       const char *device_path)
{
  bluez_operation_reporter_device_result(reporter,
                                         BLUEZ_OPERATION_PAIR_DEVICE,
                                         success,
                                         message,
                                         device_path);
}

void
bluez_pair_trust_connect_report_pairing(BluezOperationReporter *reporter)
{
  bluez_operation_reporter_status(reporter, bluez_operation_message_pairing_device());
}

void
bluez_pair_trust_connect_report_turning_power_on(BluezOperationReporter *reporter)
{
  bluez_operation_reporter_status(reporter, bluez_operation_message_turning_bluetooth_on());
}

void
bluez_pair_trust_connect_report_pair_success(BluezOperationReporter *reporter,
                                             const char *device_path)
{
  bluez_pair_trust_connect_report_result(reporter,
                                         TRUE,
                                         bluez_operation_message_device_paired(),
                                         device_path);
}

void
bluez_pair_trust_connect_report_pair_failure(BluezOperationReporter *reporter,
                                             const char *device_path,
                                             const GError *error)
{
  g_autofree char *message = bluez_operation_message_pair_failed(error ? error->message : NULL);

  bluez_pair_trust_connect_report_result(reporter, FALSE, message, device_path);
}

void
bluez_pair_trust_connect_report_system_bus_unavailable(BluezOperationReporter *reporter,
                                                       const char *device_path)
{
  bluez_pair_trust_connect_report_result(reporter,
                                         FALSE,
                                         bluez_operation_message_system_bus_unavailable(),
                                         device_path);
}

void
bluez_pair_trust_connect_report_power_failure(BluezOperationReporter *reporter,
                                              const char *device_path,
                                              BluezPowerGateStatus status)
{
  bluez_device_power_gate_report_failure(reporter,
                                         BLUEZ_OPERATION_PAIR_DEVICE,
                                         "pair",
                                         device_path,
                                         status);
}
