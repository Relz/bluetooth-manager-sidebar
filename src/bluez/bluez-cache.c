#include "bluez/bluez-cache.h"

#include "bluez/bluez-cache-private.h"
#include "bluez/bluez-cache-status.h"
#include "bluez/bluez-cache-updater.h"
#include "bluez/bluez-object-path.h"
#include "bluez/bluez-object-store.h"

struct _BluezObjectCache {
  BluezObjectStore *store;
  BluezCacheStatus *status;
};

gboolean
bluez_object_cache_path_is_valid(const char *path)
{
  return bluez_object_path_is_valid(path);
}

gboolean
bluez_object_cache_contains_adapter(BluezObjectCache *cache, const char *path)
{
  return bluez_object_store_contains_adapter(cache ? cache->store : NULL, path);
}

char **
bluez_object_cache_dup_adapter_paths_sorted(BluezObjectCache *cache)
{
  return bluez_object_store_dup_adapter_paths_sorted(cache ? cache->store : NULL);
}

char **
bluez_object_cache_dup_device_paths_sorted(BluezObjectCache *cache)
{
  return bluez_object_store_dup_device_paths_sorted(cache ? cache->store : NULL);
}

void
bluez_object_cache_replace_adapter(BluezObjectCache *cache,
                                    const char *path,
                                    BluezProperties *properties)
{
  if (!cache) {
    bluez_properties_free(properties);
    return;
  }

  bluez_object_store_replace_adapter(cache->store, path, properties);
}

void
bluez_object_cache_replace_device(BluezObjectCache *cache,
                                   const char *path,
                                   BluezProperties *properties)
{
  if (!cache) {
    bluez_properties_free(properties);
    return;
  }

  bluez_object_store_replace_device(cache->store, path, properties);
}

gboolean
bluez_object_cache_remove_adapter(BluezObjectCache *cache, const char *path)
{
  return bluez_object_store_remove_adapter(cache ? cache->store : NULL, path);
}

void
bluez_object_cache_remove_device(BluezObjectCache *cache, const char *path)
{
  bluez_object_store_remove_device(cache ? cache->store : NULL, path);
}

void
bluez_object_cache_set_device_battery(BluezObjectCache *cache, const char *path, int percentage)
{
  bluez_object_store_set_device_battery(cache ? cache->store : NULL, path, percentage);
}

void
bluez_object_cache_remove_device_battery(BluezObjectCache *cache, const char *path)
{
  bluez_object_store_remove_device_battery(cache ? cache->store : NULL, path);
}

int
bluez_object_cache_device_battery_value(BluezObjectCache *cache,
                                         const char *path,
                                         gboolean *found)
{
  return bluez_object_store_device_battery_value(cache ? cache->store : NULL, path, found);
}

void
bluez_object_cache_set_primary_adapter_path(BluezObjectCache *cache, const char *path)
{
  bluez_cache_status_set_primary_adapter(cache ? cache->status : NULL, path);
}

BluezObjectCache *
bluez_object_cache_new(void)
{
  BluezObjectCache *cache = g_new0(BluezObjectCache, 1);

  cache->store = bluez_object_store_new();
  cache->status = bluez_cache_status_new();

  return cache;
}

void
bluez_object_cache_free(BluezObjectCache *cache)
{
  if (!cache)
    return;

  bluez_object_store_free(cache->store);
  bluez_cache_status_free(cache->status);
  g_free(cache);
}

void
bluez_object_cache_clear(BluezObjectCache *cache)
{
  if (!cache)
    return;

  bluez_object_store_clear(cache->store);
  bluez_cache_status_clear_primary_adapter(cache->status);
}

void
bluez_object_cache_replace_from_managed_objects(BluezObjectCache *cache,
                                                GVariant *managed_objects)
{
  if (!cache)
    return;

  bluez_object_cache_clear(cache);
  bluez_cache_updater_replace_from_managed_objects(cache, managed_objects);
  bluez_cache_status_set_error(cache->status, NULL);
}

void
bluez_object_cache_cache_interfaces(BluezObjectCache *cache,
                                    const char *path,
                                    GVariant *interfaces)
{
  bluez_cache_updater_cache_interfaces(cache, path, interfaces);
}

void
bluez_object_cache_remove_interfaces(BluezObjectCache *cache,
                                     const char *path,
                                     GVariant *interfaces)
{
  bluez_cache_updater_remove_interfaces(cache, path, interfaces);
}

void
bluez_object_cache_apply_properties_changed(BluezObjectCache *cache,
                                            const char *object_path,
                                            const char *changed_interface,
                                            GVariant *changed,
                                            GVariant *invalidated)
{
  bluez_cache_updater_apply_properties_changed(cache,
                                               object_path,
                                               changed_interface,
                                               changed,
                                               invalidated);
}

const char *
bluez_object_cache_primary_adapter(BluezObjectCache *cache)
{
  return bluez_cache_status_primary_adapter(cache ? cache->status : NULL);
}

BluezProperties *
bluez_object_cache_adapter(BluezObjectCache *cache, const char *path)
{
  return bluez_object_store_adapter(cache ? cache->store : NULL, path);
}

BluezProperties *
bluez_object_cache_device(BluezObjectCache *cache, const char *path)
{
  return bluez_object_store_device(cache ? cache->store : NULL, path);
}

void
bluez_object_cache_set_error(BluezObjectCache *cache, const char *message)
{
  bluez_cache_status_set_error(cache ? cache->status : NULL, message);
}

const char *
bluez_object_cache_error(BluezObjectCache *cache)
{
  return bluez_cache_status_error(cache ? cache->status : NULL);
}
