#include "bluez/bluez-device-power-report.h"

#include "bluez/bluez-operation-messages.h"

gboolean
bluez_device_power_gate_report_failure(BluezOperationReporter *reporter,
                                       const char *operation,
                                       const char *action,
                                       const char *device_path,
                                       BluezPowerGateStatus status)
{
  g_autofree char *message = NULL;
  const char *static_message = NULL;

  if (!reporter || !operation || !device_path)
    return FALSE;

  switch (status) {
    case BLUEZ_POWER_GATE_STATUS_SYSTEM_BUS_UNAVAILABLE:
      static_message = bluez_operation_message_system_bus_unavailable();
      break;
    case BLUEZ_POWER_GATE_STATUS_ADAPTER_UNAVAILABLE:
      message = bluez_operation_message_device_adapter_unavailable(action);
      break;
    case BLUEZ_POWER_GATE_STATUS_POWER_ON_PENDING:
      static_message = bluez_operation_message_power_on_pending();
      break;
    case BLUEZ_POWER_GATE_STATUS_POWER_ON_FAILED:
      message = bluez_operation_message_device_power_failed(action);
      break;
    case BLUEZ_POWER_GATE_STATUS_POWER_ON_REQUEST_FAILED:
    case BLUEZ_POWER_GATE_STATUS_POWERING_ON:
    case BLUEZ_POWER_GATE_STATUS_READY:
    default:
      return FALSE;
  }

  bluez_operation_reporter_device_result(reporter,
                                         operation,
                                         FALSE,
                                         message ? message : static_message,
                                         device_path);
  return TRUE;
}
