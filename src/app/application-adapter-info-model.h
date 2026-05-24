#pragma once

#include "app/application.h"

#include "bluez/bluez-client.h"
#include "bluez/bluez-properties.h"

typedef struct {
  const char *adapter_path;
  BluezProperties *adapter;
  char *status_message;
} ApplicationAdapterInfoModel;

ApplicationAdapterInfoModel application_adapter_info_model_new(Application *self,
                                                                  BluezClient *bluez);
void application_adapter_info_model_clear(ApplicationAdapterInfoModel *model);
