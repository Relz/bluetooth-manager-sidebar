#pragma once

#include <glib.h>

#include "bluez/bluez-state.h"

const char *bluez_discovery_state_primary_adapter(BluezState *state);
gboolean bluez_discovery_state_adapter_is_discovering(BluezState *state,
                                                      const char *adapter_path);
gboolean bluez_discovery_state_adapter_is_powered(BluezState *state,
                                                  const char *adapter_path);
gboolean bluez_discovery_state_has_powered_adapter(BluezState *state,
                                                   const char **out_adapter_path);
