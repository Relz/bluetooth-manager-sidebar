#include "bluez/bluez-adapter-power-workflow.h"

#include "bluez/bluez-adapter-power-recovery.h"
#include "bluez/bluez-operation-ids.h"

typedef struct {
  BluezState *state;
  BluezOperationReporter *reporter;
  const char *adapter_path;
  gboolean powered;
} BluezAdapterPowerWorkflowContext;

static void
bluez_adapter_power_workflow_report_status(BluezAdapterPowerWorkflowContext *context,
                                           const char *message)
{
  bluez_operation_reporter_status(context->reporter, message);
}

static void
bluez_adapter_power_workflow_report_failure(BluezAdapterPowerWorkflowContext *context,
                                            const char *message)
{
  bluez_operation_reporter_result(context->reporter,
                                  BLUEZ_OPERATION_SET_POWER,
                                  FALSE,
                                  message,
                                  NULL,
                                  context->adapter_path,
                                  g_variant_new_boolean(context->powered));
  if (context->state)
    bluez_state_refresh(context->state);
}

static gboolean
bluez_adapter_power_workflow_run_recovery(BluezAdapterPowerWorkflowContext *context,
                                          BluezProperties *adapter)
{
  BluezAdapterPowerRecoveryResult recovery = { 0 };

  if (!bluez_adapter_power_recovery_needed(adapter))
    return TRUE;

  bluez_adapter_power_workflow_report_status(context,
                                             bluez_adapter_power_recovery_status_message());
  recovery = bluez_adapter_power_recovery_run(adapter);
  if (recovery.succeeded) {
    bluez_adapter_power_recovery_result_clear(&recovery);
    return TRUE;
  }

  bluez_adapter_power_workflow_report_failure(context, recovery.failure_message);
  bluez_adapter_power_recovery_result_clear(&recovery);
  return FALSE;
}

gboolean
bluez_adapter_power_workflow_set_power_for_adapter(BluezState *state,
                                                   BluezOperationReporter *reporter,
                                                   BluezAdapterController *controller,
                                                   const char *adapter_path,
                                                   gboolean powered,
                                                   BluezAdapterPowerFinished finished,
                                                   gpointer user_data,
                                                   GDestroyNotify destroy)
{
  BluezProperties *adapter = NULL;
  BluezAdapterPowerWorkflowContext context = {
    .state = state,
    .reporter = reporter,
    .adapter_path = adapter_path,
    .powered = powered,
  };

  if (!controller) {
    if (destroy && user_data)
      destroy(user_data);
    return FALSE;
  }

  if (powered && state && adapter_path) {
    adapter = bluez_object_cache_adapter(bluez_state_cache(state), adapter_path);
    if (!bluez_adapter_power_workflow_run_recovery(&context, adapter)) {
      if (destroy && user_data)
        destroy(user_data);
      return FALSE;
    }
  }

  return bluez_adapter_controller_set_power_for_adapter(controller,
                                                       adapter_path,
                                                       powered,
                                                       finished,
                                                       user_data,
                                                       destroy);
}

gboolean
bluez_adapter_power_workflow_set_power(BluezState *state,
                                       BluezOperationReporter *reporter,
                                       BluezAdapterController *controller,
                                       gboolean powered)
{
  return bluez_adapter_power_workflow_set_power_for_adapter(state,
                                                           reporter,
                                                            controller,
                                                            state ? bluez_state_primary_adapter(state) : NULL,
                                                            powered,
                                                            NULL,
                                                            NULL,
                                                            NULL);
}
