#pragma once

#include "app/application.h"

#include "bluez/bluez-client.h"

typedef struct {
  BluezClientStateChanged state_changed;
  BluezClientStatus status;
  BluezClientOperationResult operation_result;
  gpointer user_data;
} ApplicationBluezCallbacks;

void application_bluez_adapter_init_callbacks(Application *self,
                                              ApplicationBluezCallbacks *callbacks);
