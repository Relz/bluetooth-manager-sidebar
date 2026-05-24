#pragma once

#include "bluez/bluez-device-power.h"

typedef struct _BluezPendingPowerOperation BluezPendingPowerOperation;

BluezPendingPowerOperation *bluez_pending_power_operation_new(const char *adapter_path,
                                                                 const char *device_path,
                                                                 BluezDeviceResume resume,
                                                                 gpointer resume_data,
                                                                 GDestroyNotify resume_destroy,
                                                                 BluezPowerGatePendingFinished pending_finished,
                                                                 gpointer pending_user_data,
                                                                 GDestroyNotify pending_destroy);
void bluez_pending_power_operation_free(BluezPendingPowerOperation *pending);
gboolean bluez_pending_power_operation_matches_adapter(BluezPendingPowerOperation *pending,
                                                       const char *adapter_path);
void bluez_pending_power_operation_finish(BluezPendingPowerOperation *pending,
                                          BluezPowerGateStatus status,
                                          gboolean resume,
                                          BluezPowerGateResumeResult *out_result);
