#include "app/application-bluez-runtime.h"

#include "app/application-bluez-adapter.h"
#include "app/application-pairing-adapter.h"

struct _ApplicationBluezRuntime {
  BluezClient *client;
};

ApplicationBluezRuntime *
application_bluez_runtime_new(Application *self)
{
  ApplicationBluezRuntime *runtime = g_new0(ApplicationBluezRuntime, 1);
  ApplicationBluezCallbacks bluez_callbacks = { 0 };
  BluezPairingAgentCallbacks pairing_callbacks = { 0 };

  application_bluez_adapter_init_callbacks(self, &bluez_callbacks);
  application_pairing_adapter_init_callbacks(self, &pairing_callbacks);
  runtime->client = bluez_client_new(bluez_callbacks.state_changed,
                                     bluez_callbacks.status,
                                     bluez_callbacks.operation_result,
                                     &pairing_callbacks,
                                     bluez_callbacks.user_data,
                                     NULL);
  return runtime;
}

gboolean
application_bluez_runtime_start(ApplicationBluezRuntime *runtime)
{
  return runtime && runtime->client && bluez_client_connect(runtime->client);
}

void
application_bluez_runtime_free(ApplicationBluezRuntime *runtime)
{
  if (!runtime)
    return;

  g_clear_pointer(&runtime->client, bluez_client_free);
  g_free(runtime);
}

BluezClient *
application_bluez_runtime_client(ApplicationBluezRuntime *runtime)
{
  return runtime ? runtime->client : NULL;
}
