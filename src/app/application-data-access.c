#include "app/application-data-access.h"

#include "app/application-bluez-service-access.h"
#include "bluez/bluez-client-query.h"

BluezObjectCache *
application_bluez_cache(Application *self)
{
  BluezClient *bluez = application_services_bluez_client(self);

  return bluez ? bluez_client_cache(bluez) : NULL;
}

BluezProperties *
application_device_properties(Application *self, const char *device_path)
{
  BluezObjectCache *cache = application_bluez_cache(self);

  return cache ? bluez_object_cache_device(cache, device_path) : NULL;
}
