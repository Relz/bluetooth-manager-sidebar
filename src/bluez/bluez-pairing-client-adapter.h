#pragma once

#include "bluez/bluez-pairing-agent.h"
#include "bluez/bluez-state.h"

typedef struct _BluezPairingClientAdapter BluezPairingClientAdapter;

typedef void (*BluezPairingClientAdapterStatus)(const char *message, gpointer user_data);
typedef void (*BluezPairingClientAdapterChanged)(gpointer user_data);

BluezPairingClientAdapter *bluez_pairing_client_adapter_new(
  BluezState *state,
  const BluezPairingAgentCallbacks *external_callbacks,
  BluezPairingClientAdapterStatus status,
  BluezPairingClientAdapterChanged changed,
  gpointer user_data);
void bluez_pairing_client_adapter_free(BluezPairingClientAdapter *adapter);

void bluez_pairing_client_adapter_init_agent_callbacks(BluezPairingClientAdapter *adapter,
                                                       BluezPairingAgentCallbacks *callbacks);
const char *bluez_pairing_client_adapter_message(BluezPairingClientAdapter *adapter);
