#include "bluez/bluez-operation-call.h"

#define BLUEZ_BUS_NAME "org.bluez"

struct _BluezOperationCallResult {
  BluezOperationCallStatus status;
  const GError *error;
};

typedef struct {
  char *object_path;
  char *interface_name;
  char *method_name;
  guint timeout_ms;
  BluezOperationCallFinished finished;
  gpointer user_data;
  GDestroyNotify destroy;
} BluezOperationCallContext;

static void
bluez_operation_call_discard_parameters(GVariant *parameters)
{
  if (parameters)
    g_variant_unref(g_variant_ref_sink(parameters));
}

static gboolean
bluez_operation_call_descriptor_valid(const BluezOperationCallDescriptor *descriptor)
{
  return descriptor && descriptor->object_path && *descriptor->object_path &&
         descriptor->interface_name && *descriptor->interface_name &&
         descriptor->method_name && *descriptor->method_name;
}

static BluezOperationCallContext *
bluez_operation_call_context_new(const BluezOperationCallDescriptor *descriptor,
                                 BluezOperationCallFinished finished,
                                 gpointer user_data,
                                 GDestroyNotify destroy)
{
  BluezOperationCallContext *context = g_new0(BluezOperationCallContext, 1);

  context->object_path = g_strdup(descriptor->object_path);
  context->interface_name = g_strdup(descriptor->interface_name);
  context->method_name = g_strdup(descriptor->method_name);
  context->timeout_ms = descriptor->timeout_ms;
  context->finished = finished;
  context->user_data = user_data;
  context->destroy = destroy;
  return context;
}

static void
bluez_operation_call_context_free(BluezOperationCallContext *context)
{
  if (!context)
    return;

  if (context->destroy && context->user_data)
    context->destroy(context->user_data);
  g_free(context->object_path);
  g_free(context->interface_name);
  g_free(context->method_name);
  g_free(context);
}

static BluezOperationCallDescriptor
bluez_operation_call_context_descriptor(BluezOperationCallContext *context)
{
  return (BluezOperationCallDescriptor){
    .object_path = context->object_path,
    .interface_name = context->interface_name,
    .method_name = context->method_name,
    .timeout_ms = context->timeout_ms,
  };
}

static void
bluez_operation_call_on_finished(GObject *source_object,
                                 GAsyncResult *async_result,
                                 gpointer user_data)
{
  BluezOperationCallContext *context = user_data;
  g_autoptr(GError) error = NULL;
  g_autoptr(GVariant) reply = NULL;
  BluezOperationCallDescriptor descriptor;
  BluezOperationCallResult result;

  reply = g_dbus_connection_call_finish(G_DBUS_CONNECTION(source_object), async_result, &error);
  descriptor = bluez_operation_call_context_descriptor(context);
  result = (BluezOperationCallResult){
    .status = reply ? BLUEZ_OPERATION_CALL_SUCCESS : BLUEZ_OPERATION_CALL_ERROR,
    .error = error,
  };
  if (context->finished)
    context->finished(&descriptor, &result, context->user_data);

  bluez_operation_call_context_free(context);
}

BluezOperationCallStart
bluez_operation_call_start(BluezState *state,
                           const BluezOperationCallDescriptor *descriptor,
                           GVariant *parameters,
                           BluezOperationCallFinished finished,
                           gpointer user_data,
                           GDestroyNotify destroy)
{
  GDBusConnection *connection = NULL;
  BluezOperationCallContext *context = NULL;

  if (!bluez_operation_call_descriptor_valid(descriptor)) {
    bluez_operation_call_discard_parameters(parameters);
    return BLUEZ_OPERATION_CALL_START_INVALID;
  }

  connection = bluez_state_connection(state);
  if (!connection) {
    bluez_operation_call_discard_parameters(parameters);
    return BLUEZ_OPERATION_CALL_START_UNAVAILABLE;
  }

  context = bluez_operation_call_context_new(descriptor, finished, user_data, destroy);
  g_dbus_connection_call(connection,
                         BLUEZ_BUS_NAME,
                         descriptor->object_path,
                         descriptor->interface_name,
                         descriptor->method_name,
                         parameters,
                         NULL,
                         G_DBUS_CALL_FLAGS_NONE,
                         descriptor->timeout_ms,
                         NULL,
                         bluez_operation_call_on_finished,
                         context);
  return BLUEZ_OPERATION_CALL_START_STARTED;
}

gboolean
bluez_operation_call_result_success(const BluezOperationCallResult *result)
{
  return result && result->status == BLUEZ_OPERATION_CALL_SUCCESS;
}

const GError *
bluez_operation_call_result_error(const BluezOperationCallResult *result)
{
  return result ? result->error : NULL;
}
