#include "bluez/bluez-discovery.h"

#include "bluez/bluez-adapter-method-call.h"
#include "bluez/bluez-adapter-power-workflow.h"
#include "bluez/bluez-discovery-reporting.h"
#include "bluez/bluez-discovery-state.h"
#include "bluez/bluez-discovery-timer.h"

#define BLUEZ_SCAN_TIMEOUT_SECONDS 20

struct _BluezDiscoveryController {
  BluezState *state;
  BluezOperationReporter *reporter;
  BluezAdapterController *adapter;
  BluezDiscoveryTimer *scan_timeout;
  guint ref_count;
  gboolean power_on_pending;
  gboolean disposed;
};

typedef struct {
  BluezDiscoveryController *controller;
  gboolean report_stopped_message;
} BluezDiscoveryCallContext;

static BluezDiscoveryController *
bluez_discovery_controller_ref(BluezDiscoveryController *controller)
{
  controller->ref_count++;
  return controller;
}

static void
bluez_discovery_controller_unref(BluezDiscoveryController *controller)
{
  controller->ref_count--;
  if (controller->ref_count == 0) {
    bluez_discovery_timer_free(controller->scan_timeout);
    g_free(controller);
  }
}

static void
bluez_discovery_controller_remove_scan_timeout(BluezDiscoveryController *controller)
{
  if (controller)
    bluez_discovery_timer_clear(controller->scan_timeout);
}

static BluezDiscoveryCallContext *
bluez_discovery_call_context_new(BluezDiscoveryController *controller,
                                 gboolean report_stopped_message)
{
  BluezDiscoveryCallContext *context = g_new0(BluezDiscoveryCallContext, 1);

  context->controller = bluez_discovery_controller_ref(controller);
  context->report_stopped_message = report_stopped_message;

  return context;
}

static void
bluez_discovery_call_context_free(BluezDiscoveryCallContext *context)
{
  if (!context)
    return;

  bluez_discovery_controller_unref(context->controller);
  g_free(context);
}

static gboolean
bluez_discovery_on_scan_timeout(gpointer user_data)
{
  BluezDiscoveryController *controller = user_data;
  const char *adapter_path = NULL;

  if (controller->disposed || !controller->state)
    return G_SOURCE_REMOVE;

  adapter_path = bluez_discovery_state_primary_adapter(controller->state);
  if (bluez_discovery_state_adapter_is_discovering(controller->state, adapter_path))
    bluez_discovery_controller_stop(controller, FALSE);

  return G_SOURCE_REMOVE;
}

static void
bluez_discovery_schedule_scan_timeout(BluezDiscoveryController *controller)
{
  bluez_discovery_timer_schedule(controller->scan_timeout,
                                 BLUEZ_SCAN_TIMEOUT_SECONDS,
                                 bluez_discovery_on_scan_timeout,
                                 bluez_discovery_controller_ref(controller),
                                 (GDestroyNotify)bluez_discovery_controller_unref);
}

static gboolean
bluez_discovery_controller_call_adapter(BluezDiscoveryController *controller,
                                        const char *adapter_path,
                                        BluezAdapterMethodCall call,
                                        gboolean report_stopped_message,
                                        BluezAdapterMethodCallFinished finished)
{
  BluezDiscoveryCallContext *context = NULL;

  if (!controller || !adapter_path)
    return FALSE;

  context = bluez_discovery_call_context_new(controller, report_stopped_message);
  if (bluez_adapter_method_call_start(controller->state,
                                      adapter_path,
                                      call,
                                      NULL,
                                      finished,
                                      context,
                                      (GDestroyNotify)bluez_discovery_call_context_free))
    return TRUE;

  bluez_discovery_call_context_free(context);
  return FALSE;
}

static void
bluez_discovery_on_start_finished(BluezAdapterMethodCall call,
                                  const char *adapter_path,
                                  gboolean success,
                                  const GError *error,
                                  gpointer user_data)
{
  BluezDiscoveryCallContext *context = user_data;
  BluezDiscoveryController *controller = context->controller;

  (void)call;
  (void)adapter_path;

  if (!controller->disposed)
    bluez_discovery_reporting_start_finished(controller->reporter, success, error);
  if (success) {
    if (!controller->disposed)
      bluez_discovery_schedule_scan_timeout(controller);
  }

  bluez_state_refresh_if_active(controller->state, controller->disposed);
}

static gboolean
bluez_discovery_controller_start_for_adapter(BluezDiscoveryController *controller,
                                             const char *adapter_path)
{
  if (!controller || controller->disposed || !controller->state || !adapter_path)
    return FALSE;

  return bluez_discovery_controller_call_adapter(controller,
                                                adapter_path,
                                                BLUEZ_ADAPTER_METHOD_CALL_START_DISCOVERY,
                                                FALSE,
                                                bluez_discovery_on_start_finished);
}

static void
bluez_discovery_on_power_on_finished(const char *adapter_path,
                                     gboolean success,
                                     gpointer user_data)
{
  BluezDiscoveryController *controller = user_data;

  if (!controller || controller->disposed)
    return;

  controller->power_on_pending = FALSE;
  if (!success)
    return;

  if (!bluez_discovery_state_adapter_is_powered(controller->state, adapter_path)) {
    bluez_discovery_reporting_requires_power(controller->reporter);
    return;
  }

  bluez_discovery_controller_start_for_adapter(controller, adapter_path);
}

static gboolean
bluez_discovery_controller_power_on_and_start(BluezDiscoveryController *controller,
                                              const char *adapter_path)
{
  if (!controller || controller->disposed || !controller->state || !adapter_path)
    return FALSE;

  if (controller->power_on_pending) {
    bluez_discovery_reporting_power_on_pending(controller->reporter);
    return FALSE;
  }

  if (!controller->adapter) {
    bluez_discovery_reporting_requires_power(controller->reporter);
    return FALSE;
  }

  controller->power_on_pending = TRUE;
  if (bluez_adapter_power_workflow_set_power_for_adapter(
        controller->state,
        controller->reporter,
        controller->adapter,
        adapter_path,
        TRUE,
        bluez_discovery_on_power_on_finished,
        bluez_discovery_controller_ref(controller),
        (GDestroyNotify)bluez_discovery_controller_unref)) {
    bluez_discovery_reporting_turning_power_on(controller->reporter);
    bluez_state_refresh(controller->state);
    return TRUE;
  }

  controller->power_on_pending = FALSE;
  return FALSE;
}

static void
bluez_discovery_on_stop_finished(BluezAdapterMethodCall call,
                                 const char *adapter_path,
                                 gboolean success,
                                 const GError *error,
                                 gpointer user_data)
{
  BluezDiscoveryCallContext *context = user_data;
  BluezDiscoveryController *controller = context->controller;

  (void)call;
  (void)adapter_path;

  if (!controller->disposed)
    bluez_discovery_reporting_stop_finished(controller->reporter,
                                           success,
                                           error,
                                           context->report_stopped_message);

  bluez_state_refresh_if_active(controller->state, controller->disposed);
}

static void
bluez_discovery_controller_dispose(BluezDiscoveryController *controller)
{
  if (!controller || controller->disposed)
    return;

  controller->disposed = TRUE;
  bluez_discovery_controller_remove_scan_timeout(controller);
  controller->state = NULL;
  controller->reporter = NULL;
  controller->adapter = NULL;
  controller->power_on_pending = FALSE;
}

BluezDiscoveryController *
bluez_discovery_controller_new(BluezState *state,
                               BluezOperationReporter *reporter,
                               BluezAdapterController *adapter)
{
  BluezDiscoveryController *controller = NULL;

  g_return_val_if_fail(state != NULL, NULL);
  g_return_val_if_fail(adapter != NULL, NULL);

  controller = g_new0(BluezDiscoveryController, 1);
  controller->state = state;
  controller->reporter = reporter;
  controller->adapter = adapter;
  controller->scan_timeout = bluez_discovery_timer_new();
  controller->ref_count = 1;

  return controller;
}

void
bluez_discovery_controller_free(BluezDiscoveryController *controller)
{
  if (!controller)
    return;

  bluez_discovery_controller_dispose(controller);
  bluez_discovery_controller_unref(controller);
}

void
bluez_discovery_controller_shutdown(BluezDiscoveryController *controller)
{
  bluez_discovery_controller_remove_scan_timeout(controller);
}

void
bluez_discovery_controller_toggle(BluezDiscoveryController *controller)
{
  const char *adapter_path = NULL;

  if (!controller || controller->disposed || !controller->state)
    return;

  adapter_path = bluez_discovery_state_primary_adapter(controller->state);
  if (!bluez_state_connection(controller->state) || !adapter_path) {
    bluez_discovery_reporting_requires_power(controller->reporter);
    return;
  }

  if (bluez_discovery_state_adapter_is_discovering(controller->state, adapter_path))
    bluez_discovery_controller_stop(controller, TRUE);
  else if (bluez_discovery_state_adapter_is_powered(controller->state, adapter_path))
    bluez_discovery_controller_start_for_adapter(controller, adapter_path);
  else
    bluez_discovery_controller_power_on_and_start(controller, adapter_path);
}

void
bluez_discovery_controller_start(BluezDiscoveryController *controller)
{
  const char *adapter_path = NULL;

  if (!controller || controller->disposed || !controller->state)
    return;

  adapter_path = bluez_discovery_state_primary_adapter(controller->state);
  bluez_discovery_controller_start_for_adapter(controller, adapter_path);
}

void
bluez_discovery_controller_stop(BluezDiscoveryController *controller,
                                gboolean report_stopped_message)
{
  const char *adapter_path = NULL;

  if (!controller || controller->disposed || !controller->state)
    return;

  adapter_path = bluez_discovery_state_primary_adapter(controller->state);
  if (!adapter_path)
    return;

  bluez_discovery_controller_call_adapter(controller,
                                         adapter_path,
                                         BLUEZ_ADAPTER_METHOD_CALL_STOP_DISCOVERY,
                                         report_stopped_message,
                                         bluez_discovery_on_stop_finished);
}
