#pragma once

#include <gio/gio.h>

#include "bluez/bluez-operation-result.h"
#include "bluez/bluez-pairing-agent.h"

typedef struct _BluezClient BluezClient;

typedef void (*BluezClientStateChanged)(gpointer user_data);
typedef void (*BluezClientStatus)(const char *message, gpointer user_data);
typedef void (*BluezClientOperationResult)(const BluezOperationResult *result,
                                             gpointer user_data);

BluezClient *bluez_client_new(BluezClientStateChanged on_state_changed,
                                     BluezClientStatus on_status,
                                     BluezClientOperationResult on_operation_result,
                                     const BluezPairingAgentCallbacks *pairing_callbacks,
                                     gpointer user_data,
                                     GDestroyNotify destroy);
void bluez_client_free(BluezClient *client);

gboolean bluez_client_connect(BluezClient *client);
void bluez_client_shutdown(BluezClient *client);
gboolean bluez_client_refresh_state(BluezClient *client);
