#include "bluez/bluez-discovery-reporting.h"

#include "bluez/bluez-operation-ids.h"
#include "bluez/bluez-operation-messages.h"

void
bluez_discovery_reporting_start_finished(BluezOperationReporter *reporter,
                                         gboolean success,
                                         const GError *error)
{
  if (success) {
    bluez_operation_reporter_simple_result(reporter,
                                           BLUEZ_OPERATION_START_DISCOVERY,
                                           TRUE,
                                           bluez_operation_message_scan_started());
    return;
  }

  g_autofree char *message = bluez_operation_message_scan_start_failed(error ? error->message : NULL);

  bluez_operation_reporter_simple_result(reporter,
                                         BLUEZ_OPERATION_START_DISCOVERY,
                                         FALSE,
                                         message);
}

void
bluez_discovery_reporting_stop_finished(BluezOperationReporter *reporter,
                                        gboolean success,
                                        const GError *error,
                                        gboolean report_stopped_message)
{
  if (success) {
    bluez_operation_reporter_simple_result(reporter,
                                           BLUEZ_OPERATION_STOP_DISCOVERY,
                                           TRUE,
                                           report_stopped_message ? bluez_operation_message_scan_stopped() : NULL);
    return;
  }

  g_autofree char *message = bluez_operation_message_scan_stop_failed(error ? error->message : NULL);

  bluez_operation_reporter_simple_result(reporter,
                                         BLUEZ_OPERATION_STOP_DISCOVERY,
                                         FALSE,
                                         message);
}

void
bluez_discovery_reporting_requires_power(BluezOperationReporter *reporter)
{
  bluez_operation_reporter_simple_result(reporter,
                                          BLUEZ_OPERATION_TOGGLE_DISCOVERY,
                                          FALSE,
                                          bluez_operation_message_scan_requires_power());
}

void
bluez_discovery_reporting_turning_power_on(BluezOperationReporter *reporter)
{
  bluez_operation_reporter_status(reporter, bluez_operation_message_turning_bluetooth_on());
}

void
bluez_discovery_reporting_power_on_pending(BluezOperationReporter *reporter)
{
  bluez_operation_reporter_simple_result(reporter,
                                         BLUEZ_OPERATION_TOGGLE_DISCOVERY,
                                         FALSE,
                                         bluez_operation_message_power_on_pending());
}
