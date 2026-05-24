#include "bluez/bluez-device-power.h"

#include "bluez/bluez-device-power-adapter.h"
#include "bluez/bluez-device-power-pending.h"
#include "bluez/bluez-device-power-state.h"

struct _BluezDevicePowerGate {
  BluezState *state;
  BluezOperationReporter *reporter;
  BluezAdapterController *adapter;
  BluezPendingPowerOperation *pending;
  guint ref_count;
  gboolean disposed;
};

static BluezPowerGateDecision
bluez_power_gate_decision(BluezPowerGateStatus status,
                          gboolean ready,
                          gboolean pending)
{
  BluezPowerGateDecision decision = { 0 };

  decision.status = status;
  decision.ready = ready;
  decision.pending = pending;
  return decision;
}

static void
bluez_device_power_gate_discard_callbacks(BluezDeviceResume resume,
                                          gpointer resume_data,
                                          GDestroyNotify resume_destroy,
                                          gpointer pending_user_data,
                                          GDestroyNotify pending_destroy)
{
  (void)resume;
  if (resume_destroy && resume_data)
    resume_destroy(resume_data);
  if (pending_destroy && pending_user_data)
    pending_destroy(pending_user_data);
}

static BluezDevicePowerGate *
bluez_device_power_gate_ref(BluezDevicePowerGate *gate)
{
  gate->ref_count++;
  return gate;
}

static void
bluez_device_power_gate_unref(BluezDevicePowerGate *gate)
{
  gate->ref_count--;
  if (gate->ref_count == 0)
    g_free(gate);
}

static void
bluez_device_power_gate_on_adapter_power_finished(const char *adapter_path,
                                                  gboolean success,
                                                  gpointer user_data)
{
  BluezDevicePowerGate *gate = user_data;
  BluezPowerGateResumeResult result = { 0 };

  if (!gate || gate->disposed)
    return;

  bluez_device_power_gate_resume_pending(gate, adapter_path, success, &result);
  bluez_power_gate_resume_result_clear(&result);
}

BluezDevicePowerGate *
bluez_device_power_gate_new(BluezState *state,
                                BluezOperationReporter *reporter,
                                BluezAdapterController *adapter)
{
  BluezDevicePowerGate *gate = NULL;

  g_return_val_if_fail(state != NULL, NULL);
  g_return_val_if_fail(adapter != NULL, NULL);

  gate = g_new0(BluezDevicePowerGate, 1);
  gate->state = state;
  gate->reporter = reporter;
  gate->adapter = adapter;
  gate->ref_count = 1;

  return gate;
}

void
bluez_device_power_gate_free(BluezDevicePowerGate *gate)
{
  if (!gate)
    return;

  gate->disposed = TRUE;
  bluez_device_power_gate_clear(gate);
  bluez_device_power_gate_unref(gate);
}

void
bluez_device_power_gate_clear(BluezDevicePowerGate *gate)
{
  if (!gate)
    return;

  g_clear_pointer(&gate->pending, bluez_pending_power_operation_free);
}

void
bluez_power_gate_resume_result_clear(BluezPowerGateResumeResult *result)
{
  if (!result)
    return;

  g_clear_pointer(&result->device_path, g_free);
  result->status = BLUEZ_POWER_GATE_STATUS_READY;
  result->resumed = FALSE;
}

BluezPowerGateDecision
bluez_device_power_gate_ensure_powered(BluezDevicePowerGate *gate,
                                            const char *device_path,
                                            BluezDeviceResume resume,
                                                                       gpointer resume_data,
                                                                       GDestroyNotify resume_destroy,
                                                                       BluezPowerGatePendingFinished pending_finished,
                                                                       gpointer pending_user_data,
                                                                       GDestroyNotify pending_destroy)
{
  if (!gate || !gate->state || !device_path) {
    bluez_device_power_gate_discard_callbacks(resume,
                                              resume_data,
                                              resume_destroy,
                                              pending_user_data,
                                              pending_destroy);
    return bluez_power_gate_decision(BLUEZ_POWER_GATE_STATUS_SYSTEM_BUS_UNAVAILABLE, FALSE, FALSE);
  }

  BluezDevicePowerState power_state = bluez_device_power_state_resolve(gate->state,
                                                                          device_path);
  if (!power_state.available) {
    BluezPowerGateStatus status = power_state.failure_status;

    if (status == BLUEZ_POWER_GATE_STATUS_ADAPTER_UNAVAILABLE)
      bluez_state_refresh(gate->state);
    bluez_device_power_gate_discard_callbacks(resume,
                                              resume_data,
                                              resume_destroy,
                                              pending_user_data,
                                              pending_destroy);
    bluez_device_power_state_clear(&power_state);
    return bluez_power_gate_decision(status, FALSE, FALSE);
  }

  if (power_state.powered) {
    bluez_device_power_gate_discard_callbacks(resume,
                                              resume_data,
                                              resume_destroy,
                                              pending_user_data,
                                              pending_destroy);
    bluez_device_power_state_clear(&power_state);
    return bluez_power_gate_decision(BLUEZ_POWER_GATE_STATUS_READY, TRUE, FALSE);
  }

  if (gate->pending) {
    bluez_device_power_gate_discard_callbacks(resume,
                                              resume_data,
                                              resume_destroy,
                                              pending_user_data,
                                              pending_destroy);
    bluez_device_power_state_clear(&power_state);
    return bluez_power_gate_decision(BLUEZ_POWER_GATE_STATUS_POWER_ON_PENDING, FALSE, FALSE);
  }

  gate->pending = bluez_pending_power_operation_new(power_state.adapter_path,
                                                          device_path,
                                                          resume,
                                                          resume_data,
                                                         resume_destroy,
                                                         pending_finished,
                                                         pending_user_data,
                                                         pending_destroy);
  if (bluez_device_power_adapter_request_on(gate->state,
                                            gate->reporter,
                                            gate->adapter,
                                            power_state.adapter_path,
                                            bluez_device_power_gate_on_adapter_power_finished,
                                            bluez_device_power_gate_ref(gate),
                                            (GDestroyNotify)bluez_device_power_gate_unref)) {
    bluez_state_refresh(gate->state);
    bluez_device_power_state_clear(&power_state);
    return bluez_power_gate_decision(BLUEZ_POWER_GATE_STATUS_POWERING_ON, FALSE, TRUE);
  }

  bluez_device_power_gate_clear(gate);
  bluez_state_refresh(gate->state);
  bluez_device_power_state_clear(&power_state);
  return bluez_power_gate_decision(BLUEZ_POWER_GATE_STATUS_POWER_ON_REQUEST_FAILED, FALSE, FALSE);
}

gboolean
bluez_device_power_gate_resume_pending(BluezDevicePowerGate *gate,
                                           const char *adapter_path,
                                           gboolean success,
                                           BluezPowerGateResumeResult *out_result)
{
  BluezPendingPowerOperation *pending = NULL;
  if (out_result) {
    out_result->device_path = NULL;
    out_result->status = BLUEZ_POWER_GATE_STATUS_READY;
    out_result->resumed = FALSE;
  }

  if (!gate || !bluez_pending_power_operation_matches_adapter(gate->pending, adapter_path))
    return FALSE;

  pending = gate->pending;
  gate->pending = NULL;

  if (!success) {
    bluez_pending_power_operation_finish(pending,
                                         BLUEZ_POWER_GATE_STATUS_POWER_ON_REQUEST_FAILED,
                                         FALSE,
                                         out_result);
    if (gate->state)
      bluez_state_refresh(gate->state);
    bluez_pending_power_operation_free(pending);
    return TRUE;
  }

  if (!bluez_device_power_state_adapter_is_powered(gate->state, adapter_path)) {
    bluez_pending_power_operation_finish(pending,
                                         BLUEZ_POWER_GATE_STATUS_POWER_ON_FAILED,
                                         FALSE,
                                         out_result);
    if (gate->state)
      bluez_state_refresh(gate->state);
    bluez_pending_power_operation_free(pending);
    return TRUE;
  }

  bluez_pending_power_operation_finish(pending,
                                       BLUEZ_POWER_GATE_STATUS_READY,
                                       TRUE,
                                       out_result);
  bluez_pending_power_operation_free(pending);
  return TRUE;
}
