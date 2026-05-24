#pragma once

#include "bluez/bluez-cache.h"

void bluez_state_errors_set_system_bus_unavailable(BluezObjectCache *cache,
                                                   const char *detail);
void bluez_state_errors_set_bluez_unavailable(BluezObjectCache *cache,
                                              const char *detail);
