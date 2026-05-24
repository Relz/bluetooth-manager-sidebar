#include "bluez/bluez-device-connection.h"

#include "bluez/bluez-device-connection-commands.h"
#include "bluez/bluez-device-connection-power.h"
#include "bluez/bluez-device-connection-report.h"
#include "bluez/bluez-device-connection-tracker.h"

struct _BluezDeviceConnectionController {
  BluezState *state;
  BluezOperationReporter *reporter;
  BluezDeviceConnectionCommands *commands;
  BluezDeviceConnectionPower *power;
  BluezDeviceConnectionTracker *tracker;
  guint ref_count;
  gboolean disposed;
};

static void bluez_device_connection_controller_unref(BluezDeviceConnectionController *controller);
static void bluez_device_connection_controller_connect_now(BluezDeviceConnectionController *controller,
                                                                const char *device_path);

static BluezDeviceConnectionController *
bluez_device_connection_controller_ref(BluezDeviceConnectionController *controller)
{
  controller->ref_count++;
  return controller;
}

static void
bluez_device_connection_controller_finalize(BluezDeviceConnectionController *controller)
{
  g_clear_pointer(&controller->power, bluez_device_connection_power_free);
  g_clear_pointer(&controller->tracker, bluez_device_connection_tracker_free);
  g_clear_pointer(&controller->commands, bluez_device_connection_commands_free);
  g_free(controller);
}

static void
bluez_device_connection_controller_unref(BluezDeviceConnectionController *controller)
{
  controller->ref_count--;
  if (controller->ref_count == 0)
    bluez_device_connection_controller_finalize(controller);
}

static void
bluez_device_connection_controller_dispose(BluezDeviceConnectionController *controller)
{
  if (!controller || controller->disposed)
    return;

  controller->disposed = TRUE;
  bluez_device_connection_controller_shutdown(controller);
  controller->state = NULL;
  controller->reporter = NULL;
}

static void
bluez_device_connection_add_connecting(BluezDeviceConnectionController *controller,
                                            const char *device_path)
{
  if (controller)
    bluez_device_connection_tracker_add(controller->tracker, device_path);
}

static void
bluez_device_connection_remove_connecting(BluezDeviceConnectionController *controller,
                                               const char *device_path)
{
  if (controller)
    bluez_device_connection_tracker_remove(controller->tracker, device_path);
}

static void
bluez_device_connection_add_connecting_callback(const char *device_path, gpointer user_data)
{
  bluez_device_connection_add_connecting(user_data, device_path);
}

static void
bluez_device_connection_remove_connecting_callback(const char *device_path, gpointer user_data)
{
  bluez_device_connection_remove_connecting(user_data, device_path);
}

static void
bluez_device_connection_resume_connect(const char *device_path, gpointer user_data)
{
  BluezDeviceConnectionController *controller = user_data;

  bluez_device_connection_controller_connect_now(controller, device_path);
}

static void
bluez_device_connection_on_connect_finished(const char *device_path,
                                            gboolean success,
                                            gpointer user_data)
{
  BluezDeviceConnectionController *controller = user_data;

  (void)success;

  bluez_device_connection_remove_connecting(controller, device_path);
}

static void
bluez_device_connection_controller_connect_now(BluezDeviceConnectionController *controller,
                                                   const char *device_path)
{
  if (!controller || controller->disposed || !controller->state || !device_path)
    return;

  if (bluez_device_connection_commands_connect(
        controller->commands,
        device_path,
        bluez_device_connection_on_connect_finished,
        bluez_device_connection_controller_ref(controller),
        (GDestroyNotify)bluez_device_connection_controller_unref)) {
    bluez_device_connection_add_connecting(controller, device_path);
    bluez_state_refresh_if_active(controller->state, controller->disposed);
  } else {
    bluez_device_connection_remove_connecting(controller, device_path);
  }
}

BluezDeviceConnectionController *
bluez_device_connection_controller_new(BluezState *state,
                                           BluezOperationReporter *reporter,
                                           BluezAdapterController *adapter)
{
  BluezDeviceConnectionController *controller = NULL;

  g_return_val_if_fail(state != NULL, NULL);
  g_return_val_if_fail(adapter != NULL, NULL);

  controller = g_new0(BluezDeviceConnectionController, 1);
  controller->state = state;
  controller->reporter = reporter;
  controller->commands = bluez_device_connection_commands_new(state, reporter);
  controller->power = bluez_device_connection_power_new(state, reporter, adapter);
  controller->tracker = bluez_device_connection_tracker_new();
  controller->ref_count = 1;

  return controller;
}

void
bluez_device_connection_controller_free(BluezDeviceConnectionController *controller)
{
  if (!controller)
    return;

  bluez_device_connection_controller_dispose(controller);
  bluez_device_connection_controller_unref(controller);
}

void
bluez_device_connection_controller_shutdown(BluezDeviceConnectionController *controller)
{
  if (!controller)
    return;

  bluez_device_connection_tracker_clear(controller->tracker);
  bluez_device_connection_power_clear(controller->power);
}

gboolean
bluez_device_connection_controller_is_connecting(BluezDeviceConnectionController *controller,
                                                     const char *device_path)
{
  return controller && bluez_device_connection_tracker_contains(controller->tracker, device_path);
}

void
bluez_device_connection_controller_connect(BluezDeviceConnectionController *controller,
                                               const char *device_path)
{
  if (!controller || controller->disposed || !controller->state || !device_path)
    return;

  if (bluez_device_connection_controller_is_connecting(controller, device_path)) {
    bluez_device_connection_report_already_in_progress(controller->reporter, device_path);
    return;
  }

  if (!bluez_device_connection_power_request_connect(controller->power,
                                                     device_path,
                                                     bluez_device_connection_resume_connect,
                                                      bluez_device_connection_controller_ref(controller),
                                                      (GDestroyNotify)bluez_device_connection_controller_unref,
                                                      bluez_device_connection_add_connecting_callback,
                                                      bluez_device_connection_remove_connecting_callback,
                                                      controller))
    return;

  bluez_device_connection_controller_connect_now(controller, device_path);
}

void
bluez_device_connection_controller_disconnect(BluezDeviceConnectionController *controller,
                                                  const char *device_path)
{
  if (!controller || controller->disposed || !controller->state || !device_path)
    return;

  bluez_device_connection_commands_disconnect(controller->commands, device_path);
}

BluezDevicePowerGate *
bluez_device_connection_controller_power_gate(BluezDeviceConnectionController *controller)
{
  return controller ? bluez_device_connection_power_gate(controller->power) : NULL;
}
