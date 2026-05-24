#pragma once

#include "bluez/bluez-client.h"

gboolean bluez_client_is_device_connecting(BluezClient *client,
                                           const char *device_path);
void bluez_client_activate_device(BluezClient *client, const char *device_path);
void bluez_client_trust_device(BluezClient *client, const char *device_path);
void bluez_client_set_device_alias(BluezClient *client,
                                   const char *device_path,
                                   const char *alias);
void bluez_client_set_device_trusted(BluezClient *client,
                                     const char *device_path,
                                     gboolean trusted);
void bluez_client_set_device_blocked(BluezClient *client,
                                     const char *device_path,
                                     gboolean blocked);
void bluez_client_forget_device(BluezClient *client, const char *device_path);
