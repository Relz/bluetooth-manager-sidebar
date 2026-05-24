#include "bluez/bluez-operation-result.h"

struct _BluezOperationResult {
  char *operation;
  gboolean success;
  char *message;
  char *device_path;
  char *adapter_path;
  GVariant *value;
};

BluezOperationResult *
bluez_operation_result_new(const char *operation,
                               gboolean success,
                               const char *message,
                               const char *device_path,
                               const char *adapter_path,
                               GVariant *value)
{
  BluezOperationResult *result = g_new0(BluezOperationResult, 1);

  result->operation = g_strdup(operation ? operation : "");
  result->success = success;
  result->message = g_strdup(message);
  result->device_path = g_strdup(device_path);
  result->adapter_path = g_strdup(adapter_path);
  if (value)
    result->value = g_variant_ref_sink(value);

  return result;
}

BluezOperationResult *
bluez_operation_result_copy(const BluezOperationResult *result)
{
  if (!result)
    return NULL;

  return bluez_operation_result_new(result->operation,
                                        result->success,
                                        result->message,
                                        result->device_path,
                                        result->adapter_path,
                                        result->value);
}

void
bluez_operation_result_free(BluezOperationResult *result)
{
  if (!result)
    return;

  g_free(result->operation);
  g_free(result->message);
  g_free(result->device_path);
  g_free(result->adapter_path);
  g_clear_pointer(&result->value, g_variant_unref);
  g_free(result);
}

const char *
bluez_operation_result_operation(const BluezOperationResult *result)
{
  return result ? result->operation : NULL;
}

gboolean
bluez_operation_result_success(const BluezOperationResult *result)
{
  return result ? result->success : FALSE;
}

const char *
bluez_operation_result_message(const BluezOperationResult *result)
{
  return result ? result->message : NULL;
}

const char *
bluez_operation_result_device_path(const BluezOperationResult *result)
{
  return result ? result->device_path : NULL;
}

const char *
bluez_operation_result_adapter_path(const BluezOperationResult *result)
{
  return result ? result->adapter_path : NULL;
}

GVariant *
bluez_operation_result_value(const BluezOperationResult *result)
{
  return result ? result->value : NULL;
}

const char *
bluez_operation_result_get_operation(const BluezOperationResult *result)
{
  return bluez_operation_result_operation(result);
}

gboolean
bluez_operation_result_get_success(const BluezOperationResult *result)
{
  return bluez_operation_result_success(result);
}

const char *
bluez_operation_result_get_message(const BluezOperationResult *result)
{
  return bluez_operation_result_message(result);
}

const char *
bluez_operation_result_get_device_path(const BluezOperationResult *result)
{
  return bluez_operation_result_device_path(result);
}

const char *
bluez_operation_result_get_adapter_path(const BluezOperationResult *result)
{
  return bluez_operation_result_adapter_path(result);
}

GVariant *
bluez_operation_result_get_value(const BluezOperationResult *result)
{
  return bluez_operation_result_value(result);
}
