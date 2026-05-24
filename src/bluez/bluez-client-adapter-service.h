#pragma once

#include "bluez/bluez-adapter.h"
#include "bluez/bluez-operation-reporter.h"
#include "bluez/bluez-state.h"

typedef struct _BluezClientAdapterService BluezClientAdapterService;

BluezClientAdapterService *bluez_client_adapter_service_new(BluezState *state,
                                                                BluezOperationReporter *reporter);
void bluez_client_adapter_service_free(BluezClientAdapterService *service);
void bluez_client_adapter_service_shutdown(BluezClientAdapterService *service);

BluezAdapterController *bluez_client_adapter_service_controller(BluezClientAdapterService *service);
void bluez_client_adapter_service_set_power(BluezClientAdapterService *service, gboolean powered);
void bluez_client_adapter_service_set_discoverable(BluezClientAdapterService *service,
                                                   gboolean discoverable);
void bluez_client_adapter_service_toggle_discovery(BluezClientAdapterService *service);
