#include "bluez/bluez-property-set-call.h"

#include "bluez/bluez-operation-call.h"

#define DBUS_PROPERTIES_INTERFACE "org.freedesktop.DBus.Properties"
#define DBUS_SET_TIMEOUT_MS 5000

typedef struct {
  char *object_path;
  char *interface_name;
  char *property_name;
  BluezPropertySetCallFinished finished;
  gpointer user_data;
  GDestroyNotify destroy;
} BluezPropertySetCallContext;

static BluezPropertySetCallContext *
bluez_property_set_call_context_new(const char *object_path,
                                    const char *interface_name,
                                    const char *property_name,
                                    BluezPropertySetCallFinished finished,
                                    gpointer user_data,
                                    GDestroyNotify destroy)
{
  BluezPropertySetCallContext *context = g_new0(BluezPropertySetCallContext, 1);

  context->object_path = g_strdup(object_path);
  context->interface_name = g_strdup(interface_name);
  context->property_name = g_strdup(property_name);
  context->finished = finished;
  context->user_data = user_data;
  context->destroy = destroy;

  return context;
}

static void
bluez_property_set_call_context_free(BluezPropertySetCallContext *context)
{
  if (!context)
    return;

  if (context->destroy && context->user_data)
    context->destroy(context->user_data);
  g_free(context->object_path);
  g_free(context->interface_name);
  g_free(context->property_name);
  g_free(context);
}

static void
bluez_property_set_call_discard_variant(GVariant *value)
{
  if (value)
    g_variant_unref(g_variant_ref_sink(value));
}

static void
bluez_property_set_call_on_finished(const BluezOperationCallDescriptor *descriptor,
                                    const BluezOperationCallResult *result,
                                    gpointer user_data)
{
  BluezPropertySetCallContext *context = user_data;

  (void)descriptor;
  if (context->finished)
    context->finished(context->object_path,
                      context->interface_name,
                      context->property_name,
                      bluez_operation_call_result_success(result),
                      bluez_operation_call_result_error(result),
                      context->user_data);
}

gboolean
bluez_property_set_call_start(BluezState *state,
                              const char *object_path,
                              const char *interface_name,
                              const char *property_name,
                              GVariant *value,
                              BluezPropertySetCallFinished finished,
                              gpointer user_data,
                              GDestroyNotify destroy)
{
  BluezPropertySetCallContext *context = NULL;
  BluezOperationCallDescriptor descriptor;
  GVariant *parameters = NULL;

  if (!object_path || !interface_name || !property_name || !value) {
    bluez_property_set_call_discard_variant(value);
    return FALSE;
  }

  context = bluez_property_set_call_context_new(object_path,
                                               interface_name,
                                               property_name,
                                               finished,
                                               user_data,
                                               destroy);
  descriptor = (BluezOperationCallDescriptor){
    .object_path = object_path,
    .interface_name = DBUS_PROPERTIES_INTERFACE,
    .method_name = "Set",
    .timeout_ms = DBUS_SET_TIMEOUT_MS,
  };
  parameters = g_variant_new("(ssv)", interface_name, property_name, value);
  if (bluez_operation_call_start(state,
                                 &descriptor,
                                 parameters,
                                 bluez_property_set_call_on_finished,
                                 context,
                                 (GDestroyNotify)bluez_property_set_call_context_free) ==
      BLUEZ_OPERATION_CALL_START_STARTED)
    return TRUE;

  context->user_data = NULL;
  context->destroy = NULL;
  bluez_property_set_call_context_free(context);
  return FALSE;
}
