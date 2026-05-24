#pragma once

#include "bluez/bluez-adapter.h"
#include "bluez/bluez-operation-reporter.h"
#include "bluez/bluez-state.h"

typedef struct _BluezClientDeviceService BluezClientDeviceService;

BluezClientDeviceService *bluez_client_device_service_new(BluezState *state,
                                                             BluezOperationReporter *reporter,
                                                             BluezAdapterController *adapter);
void bluez_client_device_service_free(BluezClientDeviceService *service);
void bluez_client_device_service_shutdown(BluezClientDeviceService *service);

gboolean bluez_client_device_service_is_connecting(BluezClientDeviceService *service,
                                                   const char *device_path);
void bluez_client_device_service_activate(BluezClientDeviceService *service, const char *device_path);
void bluez_client_device_service_trust(BluezClientDeviceService *service, const char *device_path);
void bluez_client_device_service_set_alias(BluezClientDeviceService *service,
                                           const char *device_path,
                                           const char *alias);
void bluez_client_device_service_set_trusted(BluezClientDeviceService *service,
                                             const char *device_path,
                                             gboolean trusted);
void bluez_client_device_service_set_blocked(BluezClientDeviceService *service,
                                             const char *device_path,
                                             gboolean blocked);
void bluez_client_device_service_forget(BluezClientDeviceService *service, const char *device_path);
