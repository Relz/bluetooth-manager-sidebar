#include "bluez/bluez-property-action-runner.h"

typedef struct {
  BluezPropertyActionState state;
  BluezPropertyActionDisposed disposed;
  BluezPropertyActionFinished finished;
  gpointer user_data;
  GDestroyNotify destroy;
} BluezPropertyActionContext;

static void
bluez_property_action_runner_discard_value(GVariant *value)
{
  if (value)
    g_variant_unref(g_variant_ref_sink(value));
}

static BluezPropertyActionContext *
bluez_property_action_context_new(BluezPropertyActionState state,
                                  BluezPropertyActionDisposed disposed,
                                  BluezPropertyActionFinished finished,
                                  gpointer user_data,
                                  GDestroyNotify destroy)
{
  BluezPropertyActionContext *context = g_new0(BluezPropertyActionContext, 1);

  context->state = state;
  context->disposed = disposed;
  context->finished = finished;
  context->user_data = user_data;
  context->destroy = destroy;
  return context;
}

static void
bluez_property_action_context_free(BluezPropertyActionContext *context)
{
  if (!context)
    return;

  if (context->destroy && context->user_data)
    context->destroy(context->user_data);
  g_free(context);
}

static gboolean
bluez_property_action_context_disposed(BluezPropertyActionContext *context)
{
  return context->disposed && context->disposed(context->user_data);
}

static BluezState *
bluez_property_action_context_state(BluezPropertyActionContext *context)
{
  return context->state ? context->state(context->user_data) : NULL;
}

static void
bluez_property_action_runner_on_finished(const BluezPropertyMutationDescriptor *descriptor,
                                         const char *object_path,
                                         gboolean success,
                                         const GError *error,
                                         gpointer user_data)
{
  BluezPropertyActionContext *context = user_data;
  gboolean disposed = bluez_property_action_context_disposed(context);

  if (context->finished)
    context->finished(descriptor,
                      object_path,
                      success,
                      error,
                      disposed,
                      context->user_data);
}

gboolean
bluez_property_action_runner_start(BluezOperationReporter *reporter,
                                   const char *object_path,
                                   const BluezPropertyMutationDescriptor *descriptor,
                                   GVariant *value,
                                   BluezPropertyActionState state,
                                   BluezPropertyActionDisposed disposed,
                                   BluezPropertyActionFinished finished,
                                   gpointer user_data,
                                   GDestroyNotify destroy)
{
  BluezPropertyActionContext *context = bluez_property_action_context_new(state,
                                                                             disposed,
                                                                             finished,
                                                                             user_data,
                                                                             destroy);
  BluezState *bluez_state = bluez_property_action_context_state(context);

  if (!bluez_state || bluez_property_action_context_disposed(context)) {
    bluez_property_action_runner_discard_value(value);
    bluez_property_action_context_free(context);
    return FALSE;
  }

  if (descriptor && descriptor->status_message)
    bluez_operation_reporter_status(reporter, descriptor->status_message);

  return bluez_property_mutation_start(bluez_state,
                                       object_path,
                                       descriptor,
                                       value,
                                       bluez_property_action_runner_on_finished,
                                       context,
                                       (GDestroyNotify)bluez_property_action_context_free);
}
