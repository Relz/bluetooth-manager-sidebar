#pragma once

#include <glib.h>

#include "bluez/bluez-properties.h"

typedef struct {
  gboolean needed;
  gboolean succeeded;
  char *failure_message;
} BluezAdapterPowerRecoveryResult;

gboolean bluez_adapter_power_recovery_needed(BluezProperties *adapter);
const char *bluez_adapter_power_recovery_status_message(void);
gboolean bluez_adapter_power_recovery_unblock(char **out_failure_message);
BluezAdapterPowerRecoveryResult bluez_adapter_power_recovery_run(BluezProperties *adapter);
void bluez_adapter_power_recovery_result_clear(BluezAdapterPowerRecoveryResult *result);
