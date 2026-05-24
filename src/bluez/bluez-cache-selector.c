#include "bluez/bluez-cache-selector.h"

#include "bluez/bluez-cache-private.h"

#define BLUEZ_PREFERRED_ADAPTER_PATH "/org/bluez/hci0"

void
bluez_cache_selector_select_primary_adapter(BluezObjectCache *cache)
{
  g_auto(GStrv) paths = NULL;

  bluez_object_cache_set_primary_adapter_path(cache, NULL);

  if (bluez_object_cache_contains_adapter(cache, BLUEZ_PREFERRED_ADAPTER_PATH)) {
    bluez_object_cache_set_primary_adapter_path(cache, BLUEZ_PREFERRED_ADAPTER_PATH);
    return;
  }

  paths = bluez_object_cache_dup_adapter_paths_sorted(cache);
  if (paths && paths[0])
    bluez_object_cache_set_primary_adapter_path(cache, paths[0]);
}
