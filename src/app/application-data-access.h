#pragma once

#include "app/application.h"
#include "bluez/bluez-cache.h"

BluezObjectCache *application_bluez_cache(Application *self);
BluezProperties *application_device_properties(Application *self,
                                                 const char *device_path);
