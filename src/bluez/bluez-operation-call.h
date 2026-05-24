#pragma once

#include <gio/gio.h>

#include "bluez/bluez-state.h"

typedef enum {
  BLUEZ_OPERATION_CALL_START_INVALID,
  BLUEZ_OPERATION_CALL_START_UNAVAILABLE,
  BLUEZ_OPERATION_CALL_START_STARTED,
} BluezOperationCallStart;

typedef enum {
  BLUEZ_OPERATION_CALL_SUCCESS,
  BLUEZ_OPERATION_CALL_ERROR,
} BluezOperationCallStatus;

typedef struct {
  const char *object_path;
  const char *interface_name;
  const char *method_name;
  guint timeout_ms;
} BluezOperationCallDescriptor;

typedef struct _BluezOperationCallResult BluezOperationCallResult;

typedef void (*BluezOperationCallFinished)(const BluezOperationCallDescriptor *descriptor,
                                             const BluezOperationCallResult *result,
                                             gpointer user_data);

BluezOperationCallStart bluez_operation_call_start(BluezState *state,
                                                      const BluezOperationCallDescriptor *descriptor,
                                                      GVariant *parameters,
                                                      BluezOperationCallFinished finished,
                                                      gpointer user_data,
                                                      GDestroyNotify destroy);
gboolean bluez_operation_call_result_success(const BluezOperationCallResult *result);
const GError *bluez_operation_call_result_error(const BluezOperationCallResult *result);
