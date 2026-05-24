#pragma once

#include "app/application.h"
#include "bluez/bluez-client.h"

typedef enum {
  APPLICATION_DEVICE_GROUP_CONNECTED,
  APPLICATION_DEVICE_GROUP_PAIRED,
  APPLICATION_DEVICE_GROUP_AVAILABLE,
} ApplicationDeviceGroup;

GPtrArray *application_device_model_collect_devices(Application *self,
                                                    BluezClient *bluez,
                                                    const char *adapter_path,
                                                    ApplicationDeviceGroup group);
