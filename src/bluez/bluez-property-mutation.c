#include "bluez/bluez-property-mutation.h"

#include "bluez/bluez-property-set-call.h"

typedef struct {
  char *interface_name;
  char *property_name;
  char *status_message;
  BluezPropertyMutationFinished finished;
  gpointer user_data;
  GDestroyNotify destroy;
} BluezPropertyMutationContext;

static BluezPropertyMutationContext *
bluez_property_mutation_context_new(const BluezPropertyMutationDescriptor *descriptor,
                                    BluezPropertyMutationFinished finished,
                                    gpointer user_data,
                                    GDestroyNotify destroy)
{
  BluezPropertyMutationContext *context = g_new0(BluezPropertyMutationContext, 1);

  context->interface_name = g_strdup(descriptor ? descriptor->interface_name : NULL);
  context->property_name = g_strdup(descriptor ? descriptor->property_name : NULL);
  context->status_message = g_strdup(descriptor ? descriptor->status_message : NULL);
  context->finished = finished;
  context->user_data = user_data;
  context->destroy = destroy;
  return context;
}

static void
bluez_property_mutation_context_free(BluezPropertyMutationContext *context)
{
  if (!context)
    return;

  if (context->destroy && context->user_data)
    context->destroy(context->user_data);
  g_free(context->interface_name);
  g_free(context->property_name);
  g_free(context->status_message);
  g_free(context);
}

static BluezPropertyMutationDescriptor
bluez_property_mutation_context_descriptor(BluezPropertyMutationContext *context)
{
  return (BluezPropertyMutationDescriptor){
    .interface_name = context->interface_name,
    .property_name = context->property_name,
    .status_message = context->status_message,
  };
}

static void
bluez_property_mutation_complete(BluezPropertyMutationContext *context,
                                 const char *object_path,
                                 gboolean success,
                                 const GError *error)
{
  BluezPropertyMutationDescriptor descriptor;

  if (!context)
    return;

  descriptor = bluez_property_mutation_context_descriptor(context);
  if (context->finished)
    context->finished(&descriptor, object_path, success, error, context->user_data);
}

static void
bluez_property_mutation_on_set_finished(const char *object_path,
                                        const char *interface_name,
                                        const char *property_name,
                                        gboolean success,
                                        const GError *error,
                                        gpointer user_data)
{
  BluezPropertyMutationContext *context = user_data;

  (void)interface_name;
  (void)property_name;
  bluez_property_mutation_complete(context, object_path, success, error);
}

gboolean
bluez_property_mutation_start(BluezState *state,
                              const char *object_path,
                              const BluezPropertyMutationDescriptor *descriptor,
                              GVariant *value,
                              BluezPropertyMutationFinished finished,
                              gpointer user_data,
                              GDestroyNotify destroy)
{
  BluezPropertyMutationContext *context = NULL;

  context = bluez_property_mutation_context_new(descriptor, finished, user_data, destroy);
  if (descriptor &&
      bluez_property_set_call_start(state,
                                    object_path,
                                    descriptor->interface_name,
                                    descriptor->property_name,
                                    value,
                                    bluez_property_mutation_on_set_finished,
                                    context,
                                    (GDestroyNotify)bluez_property_mutation_context_free))
    return TRUE;

  bluez_property_mutation_complete(context, object_path, FALSE, NULL);
  bluez_property_mutation_context_free(context);
  return FALSE;
}
