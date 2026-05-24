#include "bluez/bluez-cache-status.h"

struct _BluezCacheStatus {
  char *primary_adapter_path;
  char *bluez_error;
};

BluezCacheStatus *
bluez_cache_status_new(void)
{
  return g_new0(BluezCacheStatus, 1);
}

void
bluez_cache_status_free(BluezCacheStatus *status)
{
  if (!status)
    return;

  g_free(status->primary_adapter_path);
  g_free(status->bluez_error);
  g_free(status);
}

void
bluez_cache_status_clear_primary_adapter(BluezCacheStatus *status)
{
  if (status)
    g_clear_pointer(&status->primary_adapter_path, g_free);
}

void
bluez_cache_status_set_primary_adapter(BluezCacheStatus *status, const char *path)
{
  if (!status)
    return;

  g_free(status->primary_adapter_path);
  status->primary_adapter_path = g_strdup(path);
}

const char *
bluez_cache_status_primary_adapter(BluezCacheStatus *status)
{
  return status ? status->primary_adapter_path : NULL;
}

void
bluez_cache_status_set_error(BluezCacheStatus *status, const char *message)
{
  if (!status)
    return;

  g_free(status->bluez_error);
  status->bluez_error = g_strdup(message);
}

const char *
bluez_cache_status_error(BluezCacheStatus *status)
{
  return status ? status->bluez_error : NULL;
}
