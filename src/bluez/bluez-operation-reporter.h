#pragma once

#include <glib.h>

#include "bluez/bluez-operation-result.h"

typedef void (*BluezStatusCallback)(const char *message, gpointer user_data);
/* The result is valid only for the duration of the callback. Copy it if needed. */
typedef void (*BluezOperationResultCallback)(const BluezOperationResult *result,
                                               gpointer user_data);

typedef struct _BluezOperationReporter BluezOperationReporter;

BluezOperationReporter *bluez_operation_reporter_new(BluezStatusCallback status_cb,
                                                            BluezOperationResultCallback result_cb,
                                                            gpointer user_data,
                                                            GDestroyNotify destroy);
void bluez_operation_reporter_free(BluezOperationReporter *reporter);
void bluez_operation_reporter_status(BluezOperationReporter *reporter,
                                         const char *message);
void bluez_operation_reporter_result(BluezOperationReporter *reporter,
                                          const char *operation,
                                          gboolean success,
                                          const char *message,
                                          const char *device_path,
                                          const char *adapter_path,
                                          GVariant *value);
void bluez_operation_reporter_simple_result(BluezOperationReporter *reporter,
                                            const char *operation,
                                            gboolean success,
                                            const char *message);
void bluez_operation_reporter_device_result(BluezOperationReporter *reporter,
                                            const char *operation,
                                            gboolean success,
                                            const char *message,
                                            const char *device_path);
void bluez_operation_reporter_adapter_boolean_result(BluezOperationReporter *reporter,
                                                      const char *operation,
                                                      gboolean success,
                                                      const char *message,
                                                      const char *adapter_path,
                                                      gboolean value);
