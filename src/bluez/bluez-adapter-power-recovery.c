#include "bluez/bluez-adapter-power-recovery.h"

#include "bluez/bluez-adapter-power-state.h"
#include "bluez/bluez-operation-messages.h"
#include "bluez/bluez-rfkill.h"

gboolean
bluez_adapter_power_recovery_needed(BluezProperties *adapter)
{
  return bluez_adapter_power_state_is_blocked(adapter);
}

const char *
bluez_adapter_power_recovery_status_message(void)
{
  return bluez_operation_message_unblocking_bluetooth();
}

static char *
bluez_adapter_power_recovery_failure_message(const RfkillUnblockResult *result)
{
  return bluez_operation_message_adapter_unblock_failed(result ? result->reason : NULL,
                                                        result ? result->detail : NULL);
}

gboolean
bluez_adapter_power_recovery_unblock(char **out_failure_message)
{
  RfkillUnblockResult result = { 0 };
  gboolean unblocked;

  unblocked = rfkill_unblock_bluetooth(&result);
  if (!unblocked && out_failure_message)
    *out_failure_message = bluez_adapter_power_recovery_failure_message(&result);
  rfkill_unblock_result_clear(&result);
  return unblocked;
}

BluezAdapterPowerRecoveryResult
bluez_adapter_power_recovery_run(BluezProperties *adapter)
{
  BluezAdapterPowerRecoveryResult result = { 0 };

  result.needed = bluez_adapter_power_recovery_needed(adapter);
  if (!result.needed) {
    result.succeeded = TRUE;
    return result;
  }

  result.succeeded = bluez_adapter_power_recovery_unblock(&result.failure_message);
  return result;
}

void
bluez_adapter_power_recovery_result_clear(BluezAdapterPowerRecoveryResult *result)
{
  if (!result)
    return;

  g_clear_pointer(&result->failure_message, g_free);
  result->needed = FALSE;
  result->succeeded = FALSE;
}
