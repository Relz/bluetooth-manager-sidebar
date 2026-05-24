#include "bluez/bluez-device-power-pending.h"

struct _BluezPendingPowerOperation {
  char *adapter_path;
  char *device_path;
  BluezDeviceResume resume;
  gpointer resume_data;
  GDestroyNotify resume_destroy;
  BluezPowerGatePendingFinished pending_finished;
  gpointer pending_user_data;
  GDestroyNotify pending_destroy;
};

BluezPendingPowerOperation *
bluez_pending_power_operation_new(const char *adapter_path,
                                  const char *device_path,
                                  BluezDeviceResume resume,
                                  gpointer resume_data,
                                  GDestroyNotify resume_destroy,
                                  BluezPowerGatePendingFinished pending_finished,
                                  gpointer pending_user_data,
                                  GDestroyNotify pending_destroy)
{
  BluezPendingPowerOperation *pending = g_new0(BluezPendingPowerOperation, 1);

  pending->adapter_path = g_strdup(adapter_path);
  pending->device_path = g_strdup(device_path);
  pending->resume = resume;
  pending->resume_data = resume_data;
  pending->resume_destroy = resume_destroy;
  pending->pending_finished = pending_finished;
  pending->pending_user_data = pending_user_data;
  pending->pending_destroy = pending_destroy;

  return pending;
}

void
bluez_pending_power_operation_free(BluezPendingPowerOperation *pending)
{
  if (!pending)
    return;

  if (pending->resume_destroy && pending->resume_data)
    pending->resume_destroy(pending->resume_data);
  if (pending->pending_destroy && pending->pending_user_data)
    pending->pending_destroy(pending->pending_user_data);
  g_free(pending->adapter_path);
  g_free(pending->device_path);
  g_free(pending);
}

gboolean
bluez_pending_power_operation_matches_adapter(BluezPendingPowerOperation *pending,
                                              const char *adapter_path)
{
  return pending && g_strcmp0(pending->adapter_path, adapter_path) == 0;
}

void
bluez_pending_power_operation_finish(BluezPendingPowerOperation *pending,
                                     BluezPowerGateStatus status,
                                     gboolean resume,
                                     BluezPowerGateResumeResult *out_result)
{
  BluezPowerGateResumeResult result = {
    .device_path = pending ? g_strdup(pending->device_path) : NULL,
    .status = status,
    .resumed = FALSE,
  };

  if (out_result) {
    out_result->device_path = NULL;
    out_result->status = status;
    out_result->resumed = FALSE;
  }

  if (!pending)
    return;

  if (status == BLUEZ_POWER_GATE_STATUS_READY && resume && pending->resume) {
    pending->resume(pending->device_path, pending->resume_data);
    result.resumed = TRUE;
  } else if (status != BLUEZ_POWER_GATE_STATUS_READY && pending->pending_finished) {
    pending->pending_finished(&result, pending->pending_user_data);
  }

  if (out_result) {
    out_result->device_path = g_strdup(result.device_path);
    out_result->status = result.status;
    out_result->resumed = result.resumed;
  }

  bluez_power_gate_resume_result_clear(&result);
}
