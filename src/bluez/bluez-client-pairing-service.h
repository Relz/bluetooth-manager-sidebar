#pragma once

#include "bluez/bluez-pairing-client-adapter.h"

typedef struct _BluezClientPairingService BluezClientPairingService;

BluezClientPairingService *bluez_client_pairing_service_new(
  BluezState *state,
  const BluezPairingAgentCallbacks *pairing_callbacks,
  BluezPairingClientAdapterStatus status,
  BluezPairingClientAdapterChanged changed,
  gpointer user_data);
void bluez_client_pairing_service_free(BluezClientPairingService *service);
void bluez_client_pairing_service_start(BluezClientPairingService *service);
void bluez_client_pairing_service_stop(BluezClientPairingService *service);
const char *bluez_client_pairing_service_message(BluezClientPairingService *service);
