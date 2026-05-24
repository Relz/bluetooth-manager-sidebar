#pragma once

#include <glib.h>

#include "bluez/bluez-adapter.h"
#include "bluez/bluez-operation-reporter.h"
#include "bluez/bluez-state.h"

typedef struct _BluezDevicePowerGate BluezDevicePowerGate;

typedef enum {
  BLUEZ_POWER_GATE_STATUS_READY,
  BLUEZ_POWER_GATE_STATUS_POWERING_ON,
  BLUEZ_POWER_GATE_STATUS_SYSTEM_BUS_UNAVAILABLE,
  BLUEZ_POWER_GATE_STATUS_ADAPTER_UNAVAILABLE,
  BLUEZ_POWER_GATE_STATUS_POWER_ON_PENDING,
  BLUEZ_POWER_GATE_STATUS_POWER_ON_REQUEST_FAILED,
  BLUEZ_POWER_GATE_STATUS_POWER_ON_FAILED,
} BluezPowerGateStatus;

typedef void (*BluezDeviceResume)(const char *device_path, gpointer user_data);

typedef struct {
  gboolean ready;
  gboolean pending;
  BluezPowerGateStatus status;
} BluezPowerGateDecision;

typedef struct {
  char *device_path;
  BluezPowerGateStatus status;
  gboolean resumed;
} BluezPowerGateResumeResult;

typedef void (*BluezPowerGatePendingFinished)(const BluezPowerGateResumeResult *result,
                                                gpointer user_data);

BluezDevicePowerGate *bluez_device_power_gate_new(BluezState *state,
                                                         BluezOperationReporter *reporter,
                                                         BluezAdapterController *adapter);
void bluez_device_power_gate_free(BluezDevicePowerGate *gate);
void bluez_device_power_gate_clear(BluezDevicePowerGate *gate);
void bluez_power_gate_resume_result_clear(BluezPowerGateResumeResult *result);

BluezPowerGateDecision bluez_device_power_gate_ensure_powered(BluezDevicePowerGate *gate,
                                                                       const char *device_path,
                                                                       BluezDeviceResume resume,
                                                                       gpointer resume_data,
                                                                       GDestroyNotify resume_destroy,
                                                                       BluezPowerGatePendingFinished pending_finished,
                                                                       gpointer pending_user_data,
                                                                       GDestroyNotify pending_destroy);
gboolean bluez_device_power_gate_resume_pending(BluezDevicePowerGate *gate,
                                                    const char *adapter_path,
                                                    gboolean success,
                                                    BluezPowerGateResumeResult *out_result);
