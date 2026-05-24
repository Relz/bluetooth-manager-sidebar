#include "bluez/bluez-messages.h"

char *
bluez_message_system_bus_unavailable(const char *detail)
{
  if (detail && *detail)
    return g_strdup_printf("System bus unavailable: %s", detail);

  return g_strdup("System bus unavailable");
}

char *
bluez_message_bluez_unavailable(const char *detail)
{
  if (detail && *detail)
    return g_strdup_printf("BlueZ unavailable: %s", detail);

  return g_strdup("BlueZ unavailable");
}
