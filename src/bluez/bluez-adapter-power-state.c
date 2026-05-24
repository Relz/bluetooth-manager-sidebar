#include "bluez/bluez-adapter-power-state.h"

#include <string.h>

gboolean
bluez_adapter_power_state_is_blocked(BluezProperties *adapter)
{
  g_autofree char *power_state = bluez_properties_dup_string(adapter, "PowerState");
  g_autofree char *normalized = power_state ? g_ascii_strdown(power_state, -1) : NULL;

  return normalized && strstr(normalized, "blocked") != NULL;
}
