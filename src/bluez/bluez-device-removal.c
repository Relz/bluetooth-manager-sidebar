#include "bluez/bluez-device-removal.h"

#include "bluez/bluez-adapter-method-call.h"
#include "bluez/bluez-cache.h"
#include "bluez/bluez-cache-query.h"
#include "bluez/bluez-device-removal-report.h"

struct _BluezDeviceRemovalController {
  BluezState *state;
  BluezOperationReporter *reporter;
  guint ref_count;
  gboolean disposed;
};

typedef struct {
  BluezDeviceRemovalController *controller;
  char *device_path;
} BluezDeviceRemovalCallContext;

static void bluez_device_removal_controller_unref(BluezDeviceRemovalController *controller);

static BluezDeviceRemovalController *
bluez_device_removal_controller_ref(BluezDeviceRemovalController *controller)
{
  controller->ref_count++;
  return controller;
}

static void
bluez_device_removal_controller_unref(BluezDeviceRemovalController *controller)
{
  controller->ref_count--;
  if (controller->ref_count == 0)
    g_free(controller);
}

static void
bluez_device_removal_controller_dispose(BluezDeviceRemovalController *controller)
{
  if (!controller || controller->disposed)
    return;

  controller->disposed = TRUE;
  controller->state = NULL;
  controller->reporter = NULL;
}

static BluezDeviceRemovalCallContext *
bluez_device_removal_call_context_new(BluezDeviceRemovalController *controller,
                                          const char *device_path)
{
  BluezDeviceRemovalCallContext *context = g_new0(BluezDeviceRemovalCallContext, 1);

  context->controller = bluez_device_removal_controller_ref(controller);
  context->device_path = g_strdup(device_path);

  return context;
}

static void
bluez_device_removal_call_context_free(BluezDeviceRemovalCallContext *context)
{
  if (!context)
    return;

  bluez_device_removal_controller_unref(context->controller);
  g_free(context->device_path);
  g_free(context);
}

static void
bluez_device_removal_on_forget_finished(BluezAdapterMethodCall call,
                                        const char *adapter_path,
                                        gboolean success,
                                        const GError *error,
                                        gpointer user_data)
{
  BluezDeviceRemovalCallContext *context = user_data;
  BluezDeviceRemovalController *controller = context->controller;

  (void)call;
  (void)adapter_path;

  if (!controller->disposed)
    bluez_device_removal_report_finished(controller->reporter,
                                         context->device_path,
                                         success,
                                         error);

  bluez_state_refresh_if_active(controller->state, controller->disposed);
}

BluezDeviceRemovalController *
bluez_device_removal_controller_new(BluezState *state,
                                        BluezOperationReporter *reporter)
{
  BluezDeviceRemovalController *controller = NULL;

  g_return_val_if_fail(state != NULL, NULL);

  controller = g_new0(BluezDeviceRemovalController, 1);
  controller->state = state;
  controller->reporter = reporter;
  controller->ref_count = 1;

  return controller;
}

void
bluez_device_removal_controller_free(BluezDeviceRemovalController *controller)
{
  if (!controller)
    return;

  bluez_device_removal_controller_dispose(controller);
  bluez_device_removal_controller_unref(controller);
}

void
bluez_device_removal_controller_forget(BluezDeviceRemovalController *controller,
                                           const char *device_path)
{
  g_autofree char *adapter_path = NULL;
  BluezDeviceRemovalCallContext *context = NULL;

  if (!controller || controller->disposed || !controller->state || !device_path)
    return;

  adapter_path = bluez_cache_query_dup_device_adapter_path(bluez_state_cache(controller->state),
                                                           device_path);
  if (!adapter_path) {
    bluez_device_removal_report_adapter_unavailable(controller->reporter, device_path);
    bluez_state_refresh(controller->state);
    return;
  }

  bluez_device_removal_report_removing(controller->reporter);
  context = bluez_device_removal_call_context_new(controller, device_path);
  if (!bluez_adapter_method_call_start(controller->state,
                                       adapter_path,
                                       BLUEZ_ADAPTER_METHOD_CALL_REMOVE_DEVICE,
                                       g_variant_new("(o)", device_path),
                                       bluez_device_removal_on_forget_finished,
                                       context,
                                       (GDestroyNotify)bluez_device_removal_call_context_free)) {
    bluez_device_removal_call_context_free(context);
  }
}
