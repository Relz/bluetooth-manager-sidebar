#include "bluez/bluez-device-connection-power.h"

#include "bluez/bluez-device-power-report.h"
#include "bluez/bluez-operation-ids.h"
#include "bluez/bluez-operation-messages.h"

struct _BluezDeviceConnectionPower {
  BluezState *state;
  BluezOperationReporter *reporter;
  BluezDevicePowerGate *gate;
};

typedef struct {
  BluezDeviceConnectionPower *power;
  BluezDeviceConnectionPendingChanged pending_finished;
  gpointer pending_user_data;
} BluezDeviceConnectionPowerPending;

static BluezDeviceConnectionPowerPending *
bluez_device_connection_power_pending_new(BluezDeviceConnectionPower *power,
                                          BluezDeviceConnectionPendingChanged pending_finished,
                                          gpointer pending_user_data)
{
  BluezDeviceConnectionPowerPending *pending = g_new0(BluezDeviceConnectionPowerPending, 1);

  pending->power = power;
  pending->pending_finished = pending_finished;
  pending->pending_user_data = pending_user_data;
  return pending;
}

static void
bluez_device_connection_power_report_status(BluezDeviceConnectionPower *power,
                                            const char *message)
{
  if (power)
    bluez_operation_reporter_status(power->reporter, message);
}

static void
bluez_device_connection_power_report_failure(BluezDeviceConnectionPower *power,
                                             const char *device_path,
                                             BluezPowerGateStatus status)
{
  if (!power || !device_path)
    return;

  bluez_device_power_gate_report_failure(power->reporter,
                                         BLUEZ_OPERATION_CONNECT_DEVICE,
                                         "connect",
                                         device_path,
                                         status);
}

static void
bluez_device_connection_power_on_pending_finished(const BluezPowerGateResumeResult *result,
                                                 gpointer user_data)
{
  BluezDeviceConnectionPowerPending *pending = user_data;

  if (!pending || !result)
    return;

  if (pending->pending_finished)
    pending->pending_finished(result->device_path, pending->pending_user_data);
  bluez_device_connection_power_report_failure(pending->power, result->device_path, result->status);
}

BluezDeviceConnectionPower *
bluez_device_connection_power_new(BluezState *state,
                                  BluezOperationReporter *reporter,
                                  BluezAdapterController *adapter)
{
  BluezDeviceConnectionPower *power = NULL;

  g_return_val_if_fail(state != NULL, NULL);
  g_return_val_if_fail(adapter != NULL, NULL);

  power = g_new0(BluezDeviceConnectionPower, 1);
  power->state = state;
  power->reporter = reporter;
  power->gate = bluez_device_power_gate_new(state, reporter, adapter);
  return power;
}

void
bluez_device_connection_power_free(BluezDeviceConnectionPower *power)
{
  if (!power)
    return;

  g_clear_pointer(&power->gate, bluez_device_power_gate_free);
  g_free(power);
}

void
bluez_device_connection_power_clear(BluezDeviceConnectionPower *power)
{
  if (power)
    bluez_device_power_gate_clear(power->gate);
}

BluezDevicePowerGate *
bluez_device_connection_power_gate(BluezDeviceConnectionPower *power)
{
  return power ? power->gate : NULL;
}

gboolean
bluez_device_connection_power_request_connect(BluezDeviceConnectionPower *power,
                                              const char *device_path,
                                              BluezDeviceConnectionResume resume,
                                              gpointer resume_data,
                                              GDestroyNotify resume_destroy,
                                              BluezDeviceConnectionPendingChanged pending_started,
                                              BluezDeviceConnectionPendingChanged pending_finished,
                                              gpointer pending_user_data)
{
  BluezPowerGateDecision decision = { 0 };
  BluezDeviceConnectionPowerPending *pending_context = NULL;

  if (!power || !device_path)
    return FALSE;

  pending_context = bluez_device_connection_power_pending_new(power,
                                                             pending_finished,
                                                             pending_user_data);
  decision = bluez_device_power_gate_ensure_powered(power->gate,
                                                    device_path,
                                                    resume,
                                                    resume_data,
                                                    resume_destroy,
                                                    bluez_device_connection_power_on_pending_finished,
                                                    pending_context,
                                                    g_free);
  if (decision.pending) {
    if (pending_started)
      pending_started(device_path, pending_user_data);
    bluez_device_connection_power_report_status(power, bluez_operation_message_turning_bluetooth_on());
    if (power->state)
      bluez_state_refresh(power->state);
    return FALSE;
  }

  if (!decision.ready)
    bluez_device_connection_power_report_failure(power, device_path, decision.status);

  return decision.ready;
}
