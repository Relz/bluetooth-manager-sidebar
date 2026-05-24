#pragma once

#include "bluez/bluez-client-adapter-service.h"
#include "bluez/bluez-client-device-service.h"
#include "bluez/bluez-client-pairing-service.h"
#include "bluez/bluez-client-query-service.h"
#include "bluez/bluez-client.h"

typedef struct _BluezClientServices BluezClientServices;

BluezClientServices *bluez_client_services_new(BluezClient *client,
                                                  const BluezPairingAgentCallbacks *pairing_callbacks);
void bluez_client_services_free(BluezClientServices *services);
gboolean bluez_client_services_connect(BluezClientServices *services);
void bluez_client_services_shutdown(BluezClientServices *services);
gboolean bluez_client_services_refresh_state(BluezClientServices *services);

BluezClientAdapterService *bluez_client_services_adapter(BluezClientServices *services);
BluezClientDeviceService *bluez_client_services_devices(BluezClientServices *services);
BluezClientPairingService *bluez_client_services_pairing(BluezClientServices *services);
BluezClientQueryService *bluez_client_services_query(BluezClientServices *services);
