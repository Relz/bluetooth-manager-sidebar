#include "bluez/bluez-device-pair-step.h"

#include "bluez/bluez-device-method-call.h"

typedef struct {
  BluezDevicePairStepFinished finished;
  gpointer user_data;
  GDestroyNotify destroy;
} BluezDevicePairStepContext;

static BluezDevicePairStepContext *
bluez_device_pair_step_context_new(BluezDevicePairStepFinished finished,
                                   gpointer user_data,
                                   GDestroyNotify destroy)
{
  BluezDevicePairStepContext *context = g_new0(BluezDevicePairStepContext, 1);

  context->finished = finished;
  context->user_data = user_data;
  context->destroy = destroy;
  return context;
}

static void
bluez_device_pair_step_context_free(BluezDevicePairStepContext *context)
{
  if (!context)
    return;

  if (context->destroy && context->user_data)
    context->destroy(context->user_data);
  g_free(context);
}

static void
bluez_device_pair_step_on_finished(BluezDeviceMethodCall call,
                                   const char *device_path,
                                   gboolean success,
                                   const GError *error,
                                   gpointer user_data)
{
  BluezDevicePairStepContext *context = user_data;

  (void)call;
  if (context->finished)
    context->finished(device_path, success, error, context->user_data);
}

gboolean
bluez_device_pair_step_start(BluezState *state,
                             const char *device_path,
                             BluezDevicePairStepFinished finished,
                             gpointer user_data,
                             GDestroyNotify destroy)
{
  BluezDevicePairStepContext *context = bluez_device_pair_step_context_new(finished,
                                                                              user_data,
                                                                              destroy);

  if (bluez_device_method_call_start(state,
                                     device_path,
                                     BLUEZ_DEVICE_METHOD_CALL_PAIR,
                                     bluez_device_pair_step_on_finished,
                                     context,
                                     (GDestroyNotify)bluez_device_pair_step_context_free))
    return TRUE;

  bluez_device_pair_step_context_free(context);
  return FALSE;
}
