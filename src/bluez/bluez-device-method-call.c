#include "bluez/bluez-device-method-call.h"

#include "bluez/bluez-operation-call.h"

#define BLUEZ_DEVICE_INTERFACE "org.bluez.Device1"
#define BLUEZ_DEVICE_METHOD_TIMEOUT_MS 30000

typedef struct {
  BluezDeviceMethodCall call;
  char *device_path;
  BluezDeviceMethodCallFinished finished;
  gpointer user_data;
  GDestroyNotify destroy;
} BluezDeviceMethodCallContext;

static const char *
bluez_device_method_call_name(BluezDeviceMethodCall call)
{
  switch (call) {
    case BLUEZ_DEVICE_METHOD_CALL_CONNECT:
      return "Connect";
    case BLUEZ_DEVICE_METHOD_CALL_DISCONNECT:
      return "Disconnect";
    case BLUEZ_DEVICE_METHOD_CALL_PAIR:
      return "Pair";
    default:
      return NULL;
  }
}

static BluezDeviceMethodCallContext *
bluez_device_method_call_context_new(const char *device_path,
                                     BluezDeviceMethodCall call,
                                     BluezDeviceMethodCallFinished finished,
                                     gpointer user_data,
                                     GDestroyNotify destroy)
{
  BluezDeviceMethodCallContext *context = g_new0(BluezDeviceMethodCallContext, 1);

  context->call = call;
  context->device_path = g_strdup(device_path);
  context->finished = finished;
  context->user_data = user_data;
  context->destroy = destroy;

  return context;
}

static void
bluez_device_method_call_context_free(BluezDeviceMethodCallContext *context)
{
  if (!context)
    return;

  if (context->destroy && context->user_data)
    context->destroy(context->user_data);
  g_free(context->device_path);
  g_free(context);
}

static void
bluez_device_method_call_on_finished(const BluezOperationCallDescriptor *descriptor,
                                     const BluezOperationCallResult *result,
                                     gpointer user_data)
{
  BluezDeviceMethodCallContext *context = user_data;

  (void)descriptor;
  if (context->finished)
    context->finished(context->call,
                      context->device_path,
                      bluez_operation_call_result_success(result),
                      bluez_operation_call_result_error(result),
                      context->user_data);
}

gboolean
bluez_device_method_call_start(BluezState *state,
                               const char *device_path,
                               BluezDeviceMethodCall call,
                               BluezDeviceMethodCallFinished finished,
                               gpointer user_data,
                               GDestroyNotify destroy)
{
  const char *method = bluez_device_method_call_name(call);
  BluezDeviceMethodCallContext *context = NULL;
  BluezOperationCallDescriptor descriptor;

  g_return_val_if_fail(device_path != NULL, FALSE);
  g_return_val_if_fail(method != NULL, FALSE);

  context = bluez_device_method_call_context_new(device_path,
                                                call,
                                                finished,
                                                user_data,
                                                destroy);
  descriptor = (BluezOperationCallDescriptor){
    .object_path = device_path,
    .interface_name = BLUEZ_DEVICE_INTERFACE,
    .method_name = method,
    .timeout_ms = BLUEZ_DEVICE_METHOD_TIMEOUT_MS,
  };
  if (bluez_operation_call_start(state,
                                 &descriptor,
                                 NULL,
                                 bluez_device_method_call_on_finished,
                                 context,
                                 (GDestroyNotify)bluez_device_method_call_context_free) ==
      BLUEZ_OPERATION_CALL_START_STARTED)
    return TRUE;

  context->user_data = NULL;
  context->destroy = NULL;
  bluez_device_method_call_context_free(context);
  return FALSE;
}
