#include "app/application-pairing-device-resolver.h"

#include "app/application-data-access.h"
#include "bluez/bluez-device-display.h"

char *
application_pairing_device_resolver_dup_name(Application *self, const char *device_path)
{
  return bluez_object_cache_dup_device_display_name(application_bluez_cache(self),
                                                    device_path,
                                                    NULL);
}
