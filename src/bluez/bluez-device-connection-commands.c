#include "bluez/bluez-device-connection-commands.h"

#include "bluez/bluez-device-connection-report.h"
#include "bluez/bluez-device-method-call.h"

struct _BluezDeviceConnectionCommands {
  BluezState *state;
  BluezOperationReporter *reporter;
  guint ref_count;
  gboolean disposed;
};

typedef struct {
  BluezDeviceConnectionCommands *commands;
  BluezDeviceConnectionCommandFinished finished;
  gpointer user_data;
  GDestroyNotify destroy;
} BluezDeviceConnectionCommandContext;

static BluezDeviceConnectionCommands *
bluez_device_connection_commands_ref(BluezDeviceConnectionCommands *commands)
{
  commands->ref_count++;
  return commands;
}

static void
bluez_device_connection_commands_unref(BluezDeviceConnectionCommands *commands)
{
  commands->ref_count--;
  if (commands->ref_count == 0)
    g_free(commands);
}

static void
bluez_device_connection_commands_dispose(BluezDeviceConnectionCommands *commands)
{
  if (!commands || commands->disposed)
    return;

  commands->disposed = TRUE;
  commands->state = NULL;
  commands->reporter = NULL;
}

static BluezDeviceConnectionCommandContext *
bluez_device_connection_command_context_new(BluezDeviceConnectionCommands *commands,
                                            BluezDeviceConnectionCommandFinished finished,
                                            gpointer user_data,
                                            GDestroyNotify destroy)
{
  BluezDeviceConnectionCommandContext *context = g_new0(BluezDeviceConnectionCommandContext, 1);

  context->commands = bluez_device_connection_commands_ref(commands);
  context->finished = finished;
  context->user_data = user_data;
  context->destroy = destroy;
  return context;
}

static void
bluez_device_connection_command_context_free(BluezDeviceConnectionCommandContext *context)
{
  if (!context)
    return;

  bluez_device_connection_commands_unref(context->commands);
  if (context->destroy && context->user_data)
    context->destroy(context->user_data);
  g_free(context);
}

static void
bluez_device_connection_command_notify_finished(BluezDeviceConnectionCommandContext *context,
                                                const char *device_path,
                                                gboolean success)
{
  if (context->finished)
    context->finished(device_path, success, context->user_data);
}

static void
bluez_device_connection_commands_on_connect_finished(BluezDeviceMethodCall call,
                                                     const char *device_path,
                                                     gboolean success,
                                                     const GError *error,
                                                     gpointer user_data)
{
  BluezDeviceConnectionCommandContext *context = user_data;
  BluezDeviceConnectionCommands *commands = context->commands;

  (void)call;

  bluez_device_connection_command_notify_finished(context, device_path, success);
  if (success)
    bluez_device_connection_report_connected(commands->reporter, device_path);
  else
    bluez_device_connection_report_connect_failed(commands->reporter, device_path, error);

  bluez_state_refresh_if_active(commands->state, commands->disposed);
}

static void
bluez_device_connection_commands_on_disconnect_finished(BluezDeviceMethodCall call,
                                                        const char *device_path,
                                                        gboolean success,
                                                        const GError *error,
                                                        gpointer user_data)
{
  BluezDeviceConnectionCommandContext *context = user_data;
  BluezDeviceConnectionCommands *commands = context->commands;

  (void)call;

  if (success)
    bluez_device_connection_report_disconnected(commands->reporter, device_path);
  else
    bluez_device_connection_report_disconnect_failed(commands->reporter, device_path, error);

  bluez_state_refresh_if_active(commands->state, commands->disposed);
}

BluezDeviceConnectionCommands *
bluez_device_connection_commands_new(BluezState *state, BluezOperationReporter *reporter)
{
  BluezDeviceConnectionCommands *commands = NULL;

  g_return_val_if_fail(state != NULL, NULL);

  commands = g_new0(BluezDeviceConnectionCommands, 1);
  commands->state = state;
  commands->reporter = reporter;
  commands->ref_count = 1;
  return commands;
}

void
bluez_device_connection_commands_free(BluezDeviceConnectionCommands *commands)
{
  if (!commands)
    return;

  bluez_device_connection_commands_dispose(commands);
  bluez_device_connection_commands_unref(commands);
}

gboolean
bluez_device_connection_commands_connect(BluezDeviceConnectionCommands *commands,
                                         const char *device_path,
                                         BluezDeviceConnectionCommandFinished finished,
                                         gpointer user_data,
                                         GDestroyNotify destroy)
{
  BluezDeviceConnectionCommandContext *context = NULL;

  if (!commands || commands->disposed || !commands->state || !device_path) {
    if (destroy && user_data)
      destroy(user_data);
    return FALSE;
  }

  bluez_device_connection_report_connecting(commands->reporter);
  if (commands->disposed || !commands->state) {
    if (destroy && user_data)
      destroy(user_data);
    return FALSE;
  }

  context = bluez_device_connection_command_context_new(commands, finished, user_data, destroy);
  if (bluez_device_method_call_start(commands->state,
                                     device_path,
                                     BLUEZ_DEVICE_METHOD_CALL_CONNECT,
                                     bluez_device_connection_commands_on_connect_finished,
                                     context,
                                     (GDestroyNotify)bluez_device_connection_command_context_free))
    return TRUE;

  bluez_device_connection_command_context_free(context);
  bluez_device_connection_report_connect_unavailable(commands->reporter, device_path);
  bluez_state_refresh_if_active(commands->state, commands->disposed);
  return FALSE;
}

gboolean
bluez_device_connection_commands_disconnect(BluezDeviceConnectionCommands *commands,
                                            const char *device_path)
{
  BluezDeviceConnectionCommandContext *context = NULL;

  if (!commands || commands->disposed || !commands->state || !device_path)
    return FALSE;

  bluez_device_connection_report_disconnecting(commands->reporter);
  if (commands->disposed || !commands->state)
    return FALSE;

  context = bluez_device_connection_command_context_new(commands, NULL, NULL, NULL);
  if (bluez_device_method_call_start(commands->state,
                                     device_path,
                                     BLUEZ_DEVICE_METHOD_CALL_DISCONNECT,
                                     bluez_device_connection_commands_on_disconnect_finished,
                                     context,
                                     (GDestroyNotify)bluez_device_connection_command_context_free))
    return TRUE;

  bluez_device_connection_command_context_free(context);
  bluez_device_connection_report_disconnect_unavailable(commands->reporter, device_path);
  return FALSE;
}
