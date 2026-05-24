#include "app/application-page-status.h"

#include "app/application-data-access.h"
#include "app/application-bluez-service-access.h"
#include "bluez/bluez-client-query.h"

gboolean
application_page_string_has_text(const char *value)
{
  return value && *value;
}

BluezProperties *
application_page_adapter_properties(Application *self, const char *adapter_path)
{
  BluezObjectCache *cache = application_bluez_cache(self);

  return cache ? bluez_object_cache_adapter(cache, adapter_path) : NULL;
}

char *
application_page_dup_status_message(Application *self)
{
  BluezClient *bluez = application_services_bluez_client(self);
  const char *error = bluez ? bluez_client_error(bluez) : NULL;

  if (application_page_string_has_text(error))
    return g_strdup(error);
  return g_strdup("No Bluetooth controller was found");
}
