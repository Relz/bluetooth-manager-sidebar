#pragma once

#include <glib.h>

typedef struct _BluezOperationResult BluezOperationResult;

BluezOperationResult *bluez_operation_result_new(const char *operation,
                                                        gboolean success,
                                                        const char *message,
                                                        const char *device_path,
                                                        const char *adapter_path,
                                                        GVariant *value);
BluezOperationResult *bluez_operation_result_copy(const BluezOperationResult *result);
void bluez_operation_result_free(BluezOperationResult *result);

const char *bluez_operation_result_operation(const BluezOperationResult *result);
gboolean bluez_operation_result_success(const BluezOperationResult *result);
const char *bluez_operation_result_message(const BluezOperationResult *result);
const char *bluez_operation_result_device_path(const BluezOperationResult *result);
const char *bluez_operation_result_adapter_path(const BluezOperationResult *result);
/* Borrowed, nullable value. */
GVariant *bluez_operation_result_value(const BluezOperationResult *result);

const char *bluez_operation_result_get_operation(const BluezOperationResult *result);
gboolean bluez_operation_result_get_success(const BluezOperationResult *result);
const char *bluez_operation_result_get_message(const BluezOperationResult *result);
const char *bluez_operation_result_get_device_path(const BluezOperationResult *result);
const char *bluez_operation_result_get_adapter_path(const BluezOperationResult *result);
/* Borrowed, nullable value. */
GVariant *bluez_operation_result_get_value(const BluezOperationResult *result);
