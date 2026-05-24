#include "bluez/bluez-device-properties.h"

#include "bluez/bluez-device-property-report.h"
#include "bluez/bluez-device-property-operation.h"
#include "bluez/bluez-property-action-runner.h"

#define BLUEZ_DEVICE_INTERFACE "org.bluez.Device1"

typedef struct _BluezDevicePropertyContext BluezDevicePropertyContext;

struct _BluezDevicePropertiesController {
  BluezState *state;
  BluezOperationReporter *reporter;
  guint ref_count;
  gboolean disposed;
};

struct _BluezDevicePropertyContext {
  BluezDevicePropertiesController *controller;
  BluezDevicePropertyOperation operation;
  char *device_path;
  char *alias;
  gboolean value;
  BluezDevicePropertyFinished finished;
  gpointer user_data;
  GDestroyNotify destroy;
};

static void bluez_device_properties_controller_unref(BluezDevicePropertiesController *controller);

static BluezDevicePropertiesController *
bluez_device_properties_controller_ref(BluezDevicePropertiesController *controller)
{
  controller->ref_count++;
  return controller;
}

static void
bluez_device_properties_controller_unref(BluezDevicePropertiesController *controller)
{
  controller->ref_count--;
  if (controller->ref_count == 0)
    g_free(controller);
}

static void
bluez_device_properties_controller_dispose(BluezDevicePropertiesController *controller)
{
  if (!controller || controller->disposed)
    return;

  controller->disposed = TRUE;
  controller->state = NULL;
  controller->reporter = NULL;
}

static BluezDevicePropertyContext *
bluez_device_property_context_new(BluezDevicePropertiesController *controller,
                                  const char *device_path,
                                  BluezDevicePropertyOperation operation)
{
  BluezDevicePropertyContext *context = g_new0(BluezDevicePropertyContext, 1);

  context->controller = bluez_device_properties_controller_ref(controller);
  context->operation = operation;
  context->device_path = g_strdup(device_path);
  return context;
}

static BluezDevicePropertyContext *
bluez_device_property_alias_context_new(BluezDevicePropertiesController *controller,
                                        const char *device_path,
                                        const char *alias)
{
  BluezDevicePropertyContext *context = bluez_device_property_context_new(controller,
                                                                            device_path,
                                                                            BLUEZ_DEVICE_PROPERTY_OPERATION_ALIAS);

  context->alias = g_strdup(alias ? alias : "");
  return context;
}

static BluezDevicePropertyContext *
bluez_device_property_bool_context_new(BluezDevicePropertiesController *controller,
                                       const char *device_path,
                                       BluezDevicePropertyOperation operation,
                                       gboolean value)
{
  BluezDevicePropertyContext *context = bluez_device_property_context_new(controller,
                                                                            device_path,
                                                                            operation);

  context->value = value;
  return context;
}

static void
bluez_device_property_context_free(BluezDevicePropertyContext *context)
{
  if (!context)
    return;

  bluez_device_properties_controller_unref(context->controller);
  if (context->destroy && context->user_data)
    context->destroy(context->user_data);
  g_free(context->device_path);
  g_free(context->alias);
  g_free(context);
}

static void
bluez_device_property_context_set_finished(BluezDevicePropertyContext *context,
                                           BluezDevicePropertyFinished finished,
                                           gpointer user_data,
                                           GDestroyNotify destroy)
{
  context->finished = finished;
  context->user_data = user_data;
  context->destroy = destroy;
}

static GVariant *
bluez_device_property_context_value(const BluezDevicePropertyContext *context)
{
  if (context->operation == BLUEZ_DEVICE_PROPERTY_OPERATION_ALIAS)
    return g_variant_new_string(context->alias ? context->alias : "");

  return g_variant_new_boolean(context->value);
}

static GVariant *
bluez_device_property_context_result_value(const BluezDevicePropertyContext *context)
{
  if (!bluez_device_property_operation_reports_value(context->operation))
    return NULL;

  return bluez_device_property_context_value(context);
}

static BluezPropertyMutationDescriptor
bluez_device_property_context_descriptor(const BluezDevicePropertyContext *context)
{
  return (BluezPropertyMutationDescriptor){
    .interface_name = BLUEZ_DEVICE_INTERFACE,
    .property_name = bluez_device_property_operation_property_name(context->operation),
    .status_message = bluez_device_property_operation_status_message(context->operation,
                                                                     context->value),
  };
}

static void
bluez_device_properties_on_set_finished(const BluezPropertyMutationDescriptor *descriptor,
                                        const char *object_path,
                                        gboolean success,
                                        const GError *error,
                                        gboolean owner_disposed,
                                        gpointer user_data)
{
  BluezDevicePropertyContext *context = user_data;
  BluezDevicePropertiesController *controller = context->controller;

  (void)object_path;
  (void)descriptor;

  if (!owner_disposed && (success || error))
    bluez_device_property_report_finished(controller->reporter,
                                          context->operation,
                                          context->device_path,
                                          context->value,
                                          bluez_device_property_context_result_value(context),
                                          success,
                                          error);
  else if (!owner_disposed)
    bluez_device_property_report_unavailable(controller->reporter,
                                             context->operation,
                                             context->device_path,
                                             bluez_device_property_context_result_value(context));

  if ((success || error) && context->finished && !owner_disposed)
    context->finished(context->device_path, success, context->user_data);

  if (success || error)
    bluez_state_refresh_if_active(controller->state, owner_disposed);
}

static BluezState *
bluez_device_property_context_state(gpointer user_data)
{
  BluezDevicePropertyContext *context = user_data;

  return context->controller->state;
}

static gboolean
bluez_device_property_context_disposed(gpointer user_data)
{
  BluezDevicePropertyContext *context = user_data;

  return context->controller->disposed;
}

static void
bluez_device_properties_controller_set_property(BluezDevicePropertiesController *controller,
                                                BluezDevicePropertyContext *context)
{
  const char *device_path = context->device_path;
  BluezPropertyMutationDescriptor descriptor = bluez_device_property_context_descriptor(context);

  if (controller->disposed || !controller->state) {
    bluez_device_property_context_free(context);
    return;
  }

  bluez_property_action_runner_start(controller->reporter,
                                     device_path,
                                     &descriptor,
                                     bluez_device_property_context_value(context),
                                     bluez_device_property_context_state,
                                     bluez_device_property_context_disposed,
                                     bluez_device_properties_on_set_finished,
                                     context,
                                     (GDestroyNotify)bluez_device_property_context_free);
}

BluezDevicePropertiesController *
bluez_device_properties_controller_new(BluezState *state,
                                       BluezOperationReporter *reporter)
{
  BluezDevicePropertiesController *controller = NULL;

  g_return_val_if_fail(state != NULL, NULL);

  controller = g_new0(BluezDevicePropertiesController, 1);
  controller->state = state;
  controller->reporter = reporter;
  controller->ref_count = 1;

  return controller;
}

void
bluez_device_properties_controller_free(BluezDevicePropertiesController *controller)
{
  if (!controller)
    return;

  bluez_device_properties_controller_dispose(controller);
  bluez_device_properties_controller_unref(controller);
}

void
bluez_device_properties_controller_trust(BluezDevicePropertiesController *controller,
                                          const char *device_path)
{
  bluez_device_properties_controller_trust_with_callback(controller,
                                                        device_path,
                                                        NULL,
                                                        NULL,
                                                        NULL);
}

void
bluez_device_properties_controller_trust_with_callback(BluezDevicePropertiesController *controller,
                                                       const char *device_path,
                                                       BluezDevicePropertyFinished finished,
                                                       gpointer user_data,
                                                       GDestroyNotify destroy)
{
  BluezDevicePropertyContext *context = NULL;

  if (!controller || controller->disposed || !controller->state || !device_path) {
    if (destroy && user_data)
      destroy(user_data);
    return;
  }

  context = bluez_device_property_bool_context_new(controller,
                                                   device_path,
                                                   BLUEZ_DEVICE_PROPERTY_OPERATION_TRUST,
                                                   TRUE);
  bluez_device_property_context_set_finished(context, finished, user_data, destroy);
  bluez_device_properties_controller_set_property(controller, context);
}

void
bluez_device_properties_controller_set_alias(BluezDevicePropertiesController *controller,
                                             const char *device_path,
                                             const char *alias)
{
  BluezDevicePropertyContext *context = NULL;

  if (!controller || controller->disposed || !controller->state || !device_path)
    return;

  context = bluez_device_property_alias_context_new(controller, device_path, alias);
  bluez_device_properties_controller_set_property(controller, context);
}

void
bluez_device_properties_controller_set_trusted(BluezDevicePropertiesController *controller,
                                              const char *device_path,
                                              gboolean trusted)
{
  BluezDevicePropertyContext *context = NULL;

  if (!controller || controller->disposed || !controller->state || !device_path)
    return;

  context = bluez_device_property_bool_context_new(controller,
                                                   device_path,
                                                   BLUEZ_DEVICE_PROPERTY_OPERATION_TRUSTED,
                                                   trusted);
  bluez_device_properties_controller_set_property(controller, context);
}

void
bluez_device_properties_controller_set_blocked(BluezDevicePropertiesController *controller,
                                              const char *device_path,
                                              gboolean blocked)
{
  BluezDevicePropertyContext *context = NULL;

  if (!controller || controller->disposed || !controller->state || !device_path)
    return;

  context = bluez_device_property_bool_context_new(controller,
                                                   device_path,
                                                   BLUEZ_DEVICE_PROPERTY_OPERATION_BLOCKED,
                                                   blocked);
  bluez_device_properties_controller_set_property(controller, context);
}
