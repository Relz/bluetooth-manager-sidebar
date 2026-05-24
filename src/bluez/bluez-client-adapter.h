#pragma once

#include "bluez/bluez-client.h"

void bluez_client_set_adapter_power(BluezClient *client, gboolean powered);
void bluez_client_set_adapter_discoverable(BluezClient *client, gboolean discoverable);
