#include "bluez/bluez-state-errors.h"

#include "bluez/bluez-messages.h"

static void
bluez_state_errors_set(BluezObjectCache *cache, char *message)
{
  bluez_object_cache_set_error(cache, message);
  g_free(message);
}

void
bluez_state_errors_set_system_bus_unavailable(BluezObjectCache *cache,
                                              const char *detail)
{
  bluez_state_errors_set(cache, bluez_message_system_bus_unavailable(detail));
}

void
bluez_state_errors_set_bluez_unavailable(BluezObjectCache *cache,
                                         const char *detail)
{
  bluez_state_errors_set(cache, bluez_message_bluez_unavailable(detail));
}
