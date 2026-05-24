#pragma once

#include "app/application.h"

#include "bluez/bluez-pairing-agent.h"

void application_pairing_adapter_init_callbacks(Application *self,
                                                BluezPairingAgentCallbacks *callbacks);
