#include "bluez/bluez-adapter-method-call.h"

#include "bluez/bluez-operation-call.h"

#define BLUEZ_ADAPTER_INTERFACE "org.bluez.Adapter1"
#define BLUEZ_ADAPTER_METHOD_TIMEOUT_MS 5000

typedef struct {
  BluezAdapterMethodCall call;
  char *adapter_path;
  BluezAdapterMethodCallFinished finished;
  gpointer user_data;
  GDestroyNotify destroy;
} BluezAdapterMethodCallContext;

static const char *
bluez_adapter_method_call_name(BluezAdapterMethodCall call)
{
  switch (call) {
    case BLUEZ_ADAPTER_METHOD_CALL_START_DISCOVERY:
      return "StartDiscovery";
    case BLUEZ_ADAPTER_METHOD_CALL_STOP_DISCOVERY:
      return "StopDiscovery";
    case BLUEZ_ADAPTER_METHOD_CALL_REMOVE_DEVICE:
      return "RemoveDevice";
    default:
      return NULL;
  }
}

static BluezAdapterMethodCallContext *
bluez_adapter_method_call_context_new(const char *adapter_path,
                                      BluezAdapterMethodCall call,
                                      BluezAdapterMethodCallFinished finished,
                                      gpointer user_data,
                                      GDestroyNotify destroy)
{
  BluezAdapterMethodCallContext *context = g_new0(BluezAdapterMethodCallContext, 1);

  context->call = call;
  context->adapter_path = g_strdup(adapter_path);
  context->finished = finished;
  context->user_data = user_data;
  context->destroy = destroy;

  return context;
}

static void
bluez_adapter_method_call_context_free(BluezAdapterMethodCallContext *context)
{
  if (!context)
    return;

  if (context->destroy && context->user_data)
    context->destroy(context->user_data);
  g_free(context->adapter_path);
  g_free(context);
}

static void
bluez_adapter_method_call_discard_parameters(GVariant *parameters)
{
  if (parameters)
    g_variant_unref(g_variant_ref_sink(parameters));
}

static void
bluez_adapter_method_call_on_finished(const BluezOperationCallDescriptor *descriptor,
                                      const BluezOperationCallResult *result,
                                      gpointer user_data)
{
  BluezAdapterMethodCallContext *context = user_data;

  (void)descriptor;
  if (context->finished)
    context->finished(context->call,
                      context->adapter_path,
                      bluez_operation_call_result_success(result),
                      bluez_operation_call_result_error(result),
                      context->user_data);
}

gboolean
bluez_adapter_method_call_start(BluezState *state,
                                const char *adapter_path,
                                BluezAdapterMethodCall call,
                                GVariant *parameters,
                                BluezAdapterMethodCallFinished finished,
                                gpointer user_data,
                                GDestroyNotify destroy)
{
  const char *method = bluez_adapter_method_call_name(call);
  BluezAdapterMethodCallContext *context = NULL;
  BluezOperationCallDescriptor descriptor;

  if (!adapter_path || !method) {
    bluez_adapter_method_call_discard_parameters(parameters);
    return FALSE;
  }

  context = bluez_adapter_method_call_context_new(adapter_path,
                                                 call,
                                                  finished,
                                                  user_data,
                                                  destroy);
  descriptor = (BluezOperationCallDescriptor){
    .object_path = adapter_path,
    .interface_name = BLUEZ_ADAPTER_INTERFACE,
    .method_name = method,
    .timeout_ms = BLUEZ_ADAPTER_METHOD_TIMEOUT_MS,
  };
  if (bluez_operation_call_start(state,
                                 &descriptor,
                                 parameters,
                                 bluez_adapter_method_call_on_finished,
                                 context,
                                 (GDestroyNotify)bluez_adapter_method_call_context_free) ==
      BLUEZ_OPERATION_CALL_START_STARTED)
    return TRUE;

  context->user_data = NULL;
  context->destroy = NULL;
  bluez_adapter_method_call_context_free(context);
  return FALSE;
}
