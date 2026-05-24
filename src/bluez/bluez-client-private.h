#pragma once

#include "bluez/bluez-client.h"

#include "bluez/bluez-client-services-private.h"

struct _BluezClient {
  BluezClientServices *services;

  BluezClientStateChanged on_state_changed;
  BluezClientStatus on_status;
  BluezClientOperationResult on_operation_result;
  gpointer user_data;
  GDestroyNotify destroy;
};

void bluez_client_notify_state_changed(BluezClient *client);
void bluez_client_notify_status(BluezClient *client, const char *message);
void bluez_client_notify_operation_result(BluezClient *client,
                                           const BluezOperationResult *result);
