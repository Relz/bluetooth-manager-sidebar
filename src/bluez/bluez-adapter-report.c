#include "bluez/bluez-adapter-report.h"

#include "bluez/bluez-operation-ids.h"
#include "bluez/bluez-operation-messages.h"

static void
bluez_adapter_report_boolean_result(BluezOperationReporter *reporter,
                                    const char *operation,
                                    gboolean success,
                                    const char *message,
                                    const char *adapter_path,
                                    gboolean value)
{
  bluez_operation_reporter_adapter_boolean_result(reporter,
                                                  operation,
                                                  success,
                                                  message,
                                                  adapter_path,
                                                  value);
}

void
bluez_adapter_report_power_result(BluezOperationReporter *reporter,
                                  gboolean success,
                                  const GError *error,
                                  const char *adapter_path,
                                  gboolean powered)
{
  if (success) {
    bluez_adapter_report_boolean_result(reporter,
                                        BLUEZ_OPERATION_SET_POWER,
                                        TRUE,
                                        bluez_operation_message_adapter_power_succeeded(powered),
                                        adapter_path,
                                        powered);
    return;
  }

  g_autofree char *message = bluez_operation_message_adapter_power_failed(powered,
                                                                          error ? error->message : NULL);
  bluez_adapter_report_boolean_result(reporter,
                                      BLUEZ_OPERATION_SET_POWER,
                                      FALSE,
                                      message,
                                      adapter_path,
                                      powered);
}

void
bluez_adapter_report_power_no_adapter(BluezOperationReporter *reporter, gboolean powered)
{
  bluez_adapter_report_boolean_result(reporter,
                                      BLUEZ_OPERATION_SET_POWER,
                                      FALSE,
                                      bluez_operation_message_no_adapter(),
                                      NULL,
                                      powered);
}

void
bluez_adapter_report_power_system_bus_unavailable(BluezOperationReporter *reporter,
                                                  const char *adapter_path,
                                                  gboolean powered)
{
  bluez_adapter_report_boolean_result(reporter,
                                      BLUEZ_OPERATION_SET_POWER,
                                      FALSE,
                                      bluez_operation_message_system_bus_unavailable(),
                                       adapter_path,
                                       powered);
}

gboolean
bluez_adapter_report_power_policy_failure(BluezOperationReporter *reporter,
                                          BluezAdapterPolicyResult policy,
                                          gboolean powered)
{
  if (policy == BLUEZ_ADAPTER_POLICY_READY)
    return FALSE;

  bluez_adapter_report_power_no_adapter(reporter, powered);
  return TRUE;
}

void
bluez_adapter_report_discoverable_result(BluezOperationReporter *reporter,
                                         gboolean success,
                                         const GError *error,
                                         const char *adapter_path,
                                         gboolean discoverable)
{
  if (success) {
    bluez_adapter_report_boolean_result(reporter,
                                        BLUEZ_OPERATION_SET_DISCOVERABLE,
                                        TRUE,
                                        bluez_operation_message_adapter_discoverable_succeeded(discoverable),
                                        adapter_path,
                                        discoverable);
    return;
  }

  g_autofree char *message = bluez_operation_message_adapter_discoverable_failed(discoverable,
                                                                                 error ? error->message : NULL);
  bluez_adapter_report_boolean_result(reporter,
                                      BLUEZ_OPERATION_SET_DISCOVERABLE,
                                      FALSE,
                                      message,
                                      adapter_path,
                                      discoverable);
}

void
bluez_adapter_report_discoverable_no_adapter(BluezOperationReporter *reporter,
                                             gboolean discoverable)
{
  bluez_adapter_report_boolean_result(reporter,
                                      BLUEZ_OPERATION_SET_DISCOVERABLE,
                                      FALSE,
                                      bluez_operation_message_no_adapter(),
                                      NULL,
                                      discoverable);
}

void
bluez_adapter_report_discoverable_requires_power(BluezOperationReporter *reporter,
                                                 const char *adapter_path,
                                                 gboolean discoverable)
{
  bluez_adapter_report_boolean_result(reporter,
                                      BLUEZ_OPERATION_SET_DISCOVERABLE,
                                      FALSE,
                                      bluez_operation_message_discoverable_requires_power(),
                                      adapter_path,
                                      discoverable);
}

void
bluez_adapter_report_discoverable_system_bus_unavailable(BluezOperationReporter *reporter,
                                                         const char *adapter_path,
                                                         gboolean discoverable)
{
  bluez_adapter_report_boolean_result(reporter,
                                      BLUEZ_OPERATION_SET_DISCOVERABLE,
                                      FALSE,
                                      bluez_operation_message_system_bus_unavailable(),
                                       adapter_path,
                                       discoverable);
}

gboolean
bluez_adapter_report_discoverable_policy_failure(BluezOperationReporter *reporter,
                                                 BluezAdapterPolicyResult policy,
                                                 const char *adapter_path,
                                                 gboolean discoverable)
{
  switch (policy) {
    case BLUEZ_ADAPTER_POLICY_READY:
      return FALSE;
    case BLUEZ_ADAPTER_POLICY_NO_ADAPTER:
      bluez_adapter_report_discoverable_no_adapter(reporter, discoverable);
      return TRUE;
    case BLUEZ_ADAPTER_POLICY_DISCOVERABLE_REQUIRES_POWER:
      bluez_adapter_report_discoverable_requires_power(reporter,
                                                       adapter_path,
                                                       discoverable);
      return TRUE;
  }

  return FALSE;
}
