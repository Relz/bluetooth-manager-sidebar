#include "bluez/bluez-pairing-client-adapter.h"

#include "bluez/bluez-device-display.h"

struct _BluezPairingClientAdapter {
  BluezState *state;
  BluezPairingAgentCallbacks external_callbacks;
  BluezPairingClientAdapterStatus status;
  BluezPairingClientAdapterChanged changed;
  gpointer user_data;
  char *message;
};

static void
bluez_pairing_client_adapter_set_message(const char *message, gpointer user_data)
{
  BluezPairingClientAdapter *adapter = user_data;

  g_free(adapter->message);
  adapter->message = g_strdup(message);
  if (message && *message && adapter->status)
    adapter->status(message, adapter->user_data);
  if (adapter->changed)
    adapter->changed(adapter->user_data);
}

static char *
bluez_pairing_client_adapter_device_name(const char *device_path, gpointer user_data)
{
  BluezPairingClientAdapter *adapter = user_data;

  if (adapter->external_callbacks.device_name) {
    char *name = adapter->external_callbacks.device_name(
      device_path,
      adapter->external_callbacks.user_data);
    if (name && *name)
      return name;
    g_free(name);
  }

  return bluez_object_cache_dup_device_display_name(bluez_state_cache(adapter->state), device_path, NULL);
}

static void
bluez_pairing_client_adapter_show_info(const char *message, gpointer user_data)
{
  BluezPairingClientAdapter *adapter = user_data;

  if (adapter->external_callbacks.show_info)
    adapter->external_callbacks.show_info(message, adapter->external_callbacks.user_data);
  else if (adapter->status)
    adapter->status(message, adapter->user_data);
}

static PairingPromptHandle *
bluez_pairing_client_adapter_request_confirmation(const char *heading,
                                                 const char *body,
                                                 PairingAccept accept,
                                                 PairingReject reject,
                                                 gpointer response_data,
                                                 GDestroyNotify response_destroy,
                                                 gpointer user_data)
{
  BluezPairingClientAdapter *adapter = user_data;

  if (!adapter->external_callbacks.request_confirmation)
    return NULL;
  return adapter->external_callbacks.request_confirmation(
    heading,
    body,
    accept,
    reject,
    response_data,
    response_destroy,
    adapter->external_callbacks.user_data);
}

static PairingPromptHandle *
bluez_pairing_client_adapter_request_pin_code(const char *heading,
                                             const char *body,
                                             PairingEntryAccept accept,
                                             PairingReject reject,
                                             gpointer response_data,
                                             GDestroyNotify response_destroy,
                                             gpointer user_data)
{
  BluezPairingClientAdapter *adapter = user_data;

  if (!adapter->external_callbacks.request_pin_code)
    return NULL;
  return adapter->external_callbacks.request_pin_code(
    heading,
    body,
    accept,
    reject,
    response_data,
    response_destroy,
    adapter->external_callbacks.user_data);
}

static PairingPromptHandle *
bluez_pairing_client_adapter_request_passkey(const char *heading,
                                            const char *body,
                                            PairingEntryAccept accept,
                                            PairingReject reject,
                                            gpointer response_data,
                                            GDestroyNotify response_destroy,
                                            gpointer user_data)
{
  BluezPairingClientAdapter *adapter = user_data;

  if (!adapter->external_callbacks.request_passkey)
    return NULL;
  return adapter->external_callbacks.request_passkey(
    heading,
    body,
    accept,
    reject,
    response_data,
    response_destroy,
    adapter->external_callbacks.user_data);
}

BluezPairingClientAdapter *
bluez_pairing_client_adapter_new(BluezState *state,
                                 const BluezPairingAgentCallbacks *external_callbacks,
                                 BluezPairingClientAdapterStatus status,
                                 BluezPairingClientAdapterChanged changed,
                                 gpointer user_data)
{
  BluezPairingClientAdapter *adapter = g_new0(BluezPairingClientAdapter, 1);

  adapter->state = state;
  adapter->status = status;
  adapter->changed = changed;
  adapter->user_data = user_data;
  if (external_callbacks)
    adapter->external_callbacks = *external_callbacks;

  return adapter;
}

void
bluez_pairing_client_adapter_free(BluezPairingClientAdapter *adapter)
{
  if (!adapter)
    return;

  g_free(adapter->message);
  g_free(adapter);
}

void
bluez_pairing_client_adapter_init_agent_callbacks(BluezPairingClientAdapter *adapter,
                                                  BluezPairingAgentCallbacks *callbacks)
{
  g_return_if_fail(callbacks != NULL);

  *callbacks = (BluezPairingAgentCallbacks){ 0 };
  if (!adapter)
    return;

  *callbacks = (BluezPairingAgentCallbacks){
    .set_agent_message = bluez_pairing_client_adapter_set_message,
    .device_name = bluez_pairing_client_adapter_device_name,
    .show_info = bluez_pairing_client_adapter_show_info,
    .request_confirmation = bluez_pairing_client_adapter_request_confirmation,
    .request_pin_code = bluez_pairing_client_adapter_request_pin_code,
    .request_passkey = bluez_pairing_client_adapter_request_passkey,
    .user_data = adapter,
  };
}

const char *
bluez_pairing_client_adapter_message(BluezPairingClientAdapter *adapter)
{
  return adapter ? adapter->message : NULL;
}
