#pragma once

#include "bluez/bluez-cache.h"

void bluez_cache_selection_refresh_primary(BluezObjectCache *cache,
                                           gboolean adapter_set_changed);
