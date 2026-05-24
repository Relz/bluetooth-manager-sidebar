#include "bluez/bluez-cache-selection.h"

#include "bluez/bluez-cache-selector.h"

void
bluez_cache_selection_refresh_primary(BluezObjectCache *cache, gboolean adapter_set_changed)
{
  if (adapter_set_changed)
    bluez_cache_selector_select_primary_adapter(cache);
}
