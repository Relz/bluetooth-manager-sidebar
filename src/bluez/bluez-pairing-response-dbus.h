#pragma once

#include "bluez/bluez-pairing-agent-private.h"

void bluez_pairing_response_dbus_discard_variant(GVariant *value);
void bluez_pairing_response_dbus_return_value(PairingResponse *response, GVariant *value);
void bluez_pairing_response_dbus_return_rejected(PairingResponse *response, const char *message);
