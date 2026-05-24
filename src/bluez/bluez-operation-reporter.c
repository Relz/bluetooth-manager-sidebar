#include "bluez/bluez-operation-reporter.h"

struct _BluezOperationReporter {
  BluezStatusCallback status_cb;
  BluezOperationResultCallback result_cb;
  gpointer user_data;
  GDestroyNotify destroy;
};

BluezOperationReporter *
bluez_operation_reporter_new(BluezStatusCallback status_cb,
                                 BluezOperationResultCallback result_cb,
                                 gpointer user_data,
                                 GDestroyNotify destroy)
{
  BluezOperationReporter *reporter = g_new0(BluezOperationReporter, 1);

  reporter->status_cb = status_cb;
  reporter->result_cb = result_cb;
  reporter->user_data = user_data;
  reporter->destroy = destroy;

  return reporter;
}

void
bluez_operation_reporter_free(BluezOperationReporter *reporter)
{
  if (!reporter)
    return;

  if (reporter->destroy && reporter->user_data)
    reporter->destroy(reporter->user_data);
  g_free(reporter);
}

void
bluez_operation_reporter_status(BluezOperationReporter *reporter,
                                    const char *message)
{
  if (!reporter || !reporter->status_cb)
    return;

  reporter->status_cb(message, reporter->user_data);
}

void
bluez_operation_reporter_result(BluezOperationReporter *reporter,
                                    const char *operation,
                                    gboolean success,
                                    const char *message,
                                    const char *device_path,
                                    const char *adapter_path,
                                    GVariant *value)
{
  BluezOperationResult *result = NULL;

  if (!reporter || !reporter->result_cb) {
    if (value)
      g_variant_unref(g_variant_ref_sink(value));
    return;
  }

  result = bluez_operation_result_new(operation,
                                          success,
                                          message,
                                          device_path,
                                          adapter_path,
                                          value);
  reporter->result_cb(result, reporter->user_data);
  bluez_operation_result_free(result);
}

void
bluez_operation_reporter_simple_result(BluezOperationReporter *reporter,
                                       const char *operation,
                                       gboolean success,
                                       const char *message)
{
  bluez_operation_reporter_result(reporter, operation, success, message, NULL, NULL, NULL);
}

void
bluez_operation_reporter_device_result(BluezOperationReporter *reporter,
                                       const char *operation,
                                       gboolean success,
                                       const char *message,
                                       const char *device_path)
{
  bluez_operation_reporter_result(reporter, operation, success, message, device_path, NULL, NULL);
}

void
bluez_operation_reporter_adapter_boolean_result(BluezOperationReporter *reporter,
                                                 const char *operation,
                                                 gboolean success,
                                                const char *message,
                                                const char *adapter_path,
                                                gboolean value)
{
  bluez_operation_reporter_result(reporter,
                                  operation,
                                  success,
                                  message,
                                  NULL,
                                  adapter_path,
                                  g_variant_new_boolean(value));
}
