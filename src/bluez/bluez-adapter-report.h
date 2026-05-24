#pragma once

#include "bluez/bluez-adapter-policy.h"
#include "bluez/bluez-operation-reporter.h"

void bluez_adapter_report_power_result(BluezOperationReporter *reporter,
                                       gboolean success,
                                       const GError *error,
                                       const char *adapter_path,
                                       gboolean powered);
void bluez_adapter_report_power_no_adapter(BluezOperationReporter *reporter,
                                           gboolean powered);
void bluez_adapter_report_power_system_bus_unavailable(BluezOperationReporter *reporter,
                                                       const char *adapter_path,
                                                       gboolean powered);
gboolean bluez_adapter_report_power_policy_failure(BluezOperationReporter *reporter,
                                                   BluezAdapterPolicyResult policy,
                                                   gboolean powered);

void bluez_adapter_report_discoverable_result(BluezOperationReporter *reporter,
                                              gboolean success,
                                              const GError *error,
                                              const char *adapter_path,
                                              gboolean discoverable);
void bluez_adapter_report_discoverable_no_adapter(BluezOperationReporter *reporter,
                                                  gboolean discoverable);
void bluez_adapter_report_discoverable_requires_power(BluezOperationReporter *reporter,
                                                      const char *adapter_path,
                                                      gboolean discoverable);
void bluez_adapter_report_discoverable_system_bus_unavailable(BluezOperationReporter *reporter,
                                                              const char *adapter_path,
                                                              gboolean discoverable);
gboolean bluez_adapter_report_discoverable_policy_failure(BluezOperationReporter *reporter,
                                                          BluezAdapterPolicyResult policy,
                                                          const char *adapter_path,
                                                          gboolean discoverable);
