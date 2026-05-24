#include "bluez/bluez-adapter.h"

#include "bluez/bluez-adapter-policy.h"
#include "bluez/bluez-adapter-report.h"
#include "bluez/bluez-property-action-runner.h"

#define BLUEZ_ADAPTER_INTERFACE "org.bluez.Adapter1"

struct _BluezAdapterController {
  BluezState *state;
  BluezOperationReporter *reporter;
  guint ref_count;
  gboolean disposed;
};

typedef struct {
  BluezAdapterController *controller;
  char *adapter_path;
  gboolean value;
  BluezAdapterPowerFinished finished;
  gpointer user_data;
  GDestroyNotify destroy;
} BluezAdapterPropertyContext;

static BluezAdapterController *
bluez_adapter_controller_ref(BluezAdapterController *controller)
{
  controller->ref_count++;
  return controller;
}

static void
bluez_adapter_controller_unref(BluezAdapterController *controller)
{
  controller->ref_count--;
  if (controller->ref_count == 0)
    g_free(controller);
}

static void
bluez_adapter_controller_dispose(BluezAdapterController *controller)
{
  if (!controller || controller->disposed)
    return;

  controller->disposed = TRUE;
  controller->state = NULL;
  controller->reporter = NULL;
}

static BluezAdapterPropertyContext *
bluez_adapter_property_context_new(BluezAdapterController *controller,
                                        const char *adapter_path,
                                        gboolean value,
                                        BluezAdapterPowerFinished finished,
                                        gpointer user_data,
                                        GDestroyNotify destroy)
{
  BluezAdapterPropertyContext *context = g_new0(BluezAdapterPropertyContext, 1);

  context->controller = bluez_adapter_controller_ref(controller);
  context->adapter_path = g_strdup(adapter_path);
  context->value = value;
  context->finished = finished;
  context->user_data = user_data;
  context->destroy = destroy;

  return context;
}

static void
bluez_adapter_property_context_free(BluezAdapterPropertyContext *context)
{
  if (!context)
    return;

  bluez_adapter_controller_unref(context->controller);
  if (context->destroy && context->user_data)
    context->destroy(context->user_data);
  g_free(context->adapter_path);
  g_free(context);
}

static void
bluez_adapter_on_set_power_finished(const BluezPropertyMutationDescriptor *descriptor,
                                    const char *object_path,
                                    gboolean success,
                                    const GError *error,
                                    gboolean owner_disposed,
                                    gpointer user_data)
{
  BluezAdapterPropertyContext *context = user_data;
  BluezAdapterController *controller = context->controller;

  (void)object_path;
  (void)descriptor;

  if (!owner_disposed && (success || error)) {
    bluez_adapter_report_power_result(controller->reporter,
                                      success,
                                      error,
                                      context->adapter_path,
                                      context->value);
  } else if (!owner_disposed) {
    bluez_adapter_report_power_system_bus_unavailable(controller->reporter,
                                                      context->adapter_path,
                                                      context->value);
  }

  if (success || error)
    bluez_state_refresh_if_active(controller->state, owner_disposed);
  if ((success || error) && context->value && !owner_disposed && context->finished)
    context->finished(context->adapter_path, success, context->user_data);
}

static void
bluez_adapter_on_set_discoverable_finished(const BluezPropertyMutationDescriptor *descriptor,
                                           const char *object_path,
                                           gboolean success,
                                           const GError *error,
                                           gboolean owner_disposed,
                                           gpointer user_data)
{
  BluezAdapterPropertyContext *context = user_data;
  BluezAdapterController *controller = context->controller;

  (void)object_path;
  (void)descriptor;

  if (!owner_disposed && (success || error)) {
    bluez_adapter_report_discoverable_result(controller->reporter,
                                             success,
                                             error,
                                             context->adapter_path,
                                             context->value);
  } else if (!owner_disposed) {
    bluez_adapter_report_discoverable_system_bus_unavailable(controller->reporter,
                                                             context->adapter_path,
                                                             context->value);
  }

  if (success || error)
    bluez_state_refresh_if_active(controller->state, owner_disposed);
}

static BluezState *
bluez_adapter_property_context_state(gpointer user_data)
{
  BluezAdapterPropertyContext *context = user_data;

  return context->controller->state;
}

static gboolean
bluez_adapter_property_context_disposed(gpointer user_data)
{
  BluezAdapterPropertyContext *context = user_data;

  return context->controller->disposed;
}

BluezAdapterController *
bluez_adapter_controller_new(BluezState *state,
                                 BluezOperationReporter *reporter)
{
  BluezAdapterController *controller = NULL;

  g_return_val_if_fail(state != NULL, NULL);

  controller = g_new0(BluezAdapterController, 1);
  controller->state = state;
  controller->reporter = reporter;
  controller->ref_count = 1;

  return controller;
}

void
bluez_adapter_controller_free(BluezAdapterController *controller)
{
  if (!controller)
    return;

  bluez_adapter_controller_dispose(controller);
  bluez_adapter_controller_unref(controller);
}

void
bluez_adapter_controller_set_power(BluezAdapterController *controller,
                                        gboolean powered)
{
  if (!controller || controller->disposed || !controller->state)
    return;

  bluez_adapter_controller_set_power_for_adapter(controller,
                                                     bluez_state_primary_adapter(controller->state),
                                                      powered,
                                                      NULL,
                                                      NULL,
                                                      NULL);
}

gboolean
bluez_adapter_controller_set_power_for_adapter(BluezAdapterController *controller,
                                                    const char *adapter_path,
                                                    gboolean powered,
                                                    BluezAdapterPowerFinished finished,
                                                    gpointer user_data,
                                                    GDestroyNotify destroy)
{
  BluezAdapterPropertyContext *context = NULL;
  BluezAdapterPolicyResult policy = BLUEZ_ADAPTER_POLICY_READY;

  if (!controller || controller->disposed || !controller->state) {
    if (destroy && user_data)
      destroy(user_data);
    return FALSE;
  }

  policy = bluez_adapter_policy_can_set_power(controller->state, adapter_path);
  if (bluez_adapter_report_power_policy_failure(controller->reporter, policy, powered)) {
    if (destroy && user_data)
      destroy(user_data);
    return FALSE;
  }

  context = bluez_adapter_property_context_new(controller,
                                               adapter_path,
                                               powered,
                                               finished,
                                               user_data,
                                               destroy);
  return bluez_property_action_runner_start(
    controller->reporter,
    adapter_path,
    &(BluezPropertyMutationDescriptor){
      .interface_name = BLUEZ_ADAPTER_INTERFACE,
      .property_name = "Powered",
    },
    g_variant_new_boolean(powered),
    bluez_adapter_property_context_state,
    bluez_adapter_property_context_disposed,
    bluez_adapter_on_set_power_finished,
    context,
    (GDestroyNotify)bluez_adapter_property_context_free);
}

void
bluez_adapter_controller_set_discoverable(BluezAdapterController *controller,
                                              gboolean discoverable)
{
  const char *adapter_path = NULL;
  BluezAdapterPropertyContext *context = NULL;
  BluezAdapterPolicyResult policy = BLUEZ_ADAPTER_POLICY_READY;

  if (!controller || controller->disposed || !controller->state)
    return;

  adapter_path = bluez_state_primary_adapter(controller->state);
  policy = bluez_adapter_policy_can_set_discoverable(controller->state,
                                                     adapter_path,
                                                     discoverable);
  if (bluez_adapter_report_discoverable_policy_failure(controller->reporter,
                                                       policy,
                                                       adapter_path,
                                                       discoverable))
    return;

  context = bluez_adapter_property_context_new(controller,
                                               adapter_path,
                                               discoverable,
                                               NULL,
                                               NULL,
                                               NULL);
  bluez_property_action_runner_start(
    controller->reporter,
    adapter_path,
    &(BluezPropertyMutationDescriptor){
      .interface_name = BLUEZ_ADAPTER_INTERFACE,
      .property_name = "Discoverable",
    },
    g_variant_new_boolean(discoverable),
    bluez_adapter_property_context_state,
    bluez_adapter_property_context_disposed,
    bluez_adapter_on_set_discoverable_finished,
    context,
    (GDestroyNotify)bluez_adapter_property_context_free);
}
