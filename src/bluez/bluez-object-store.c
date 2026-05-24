#include "bluez/bluez-object-store.h"

#include "bluez/bluez-battery-cache.h"
#include "bluez/bluez-object-path.h"

struct _BluezObjectStore {
  GHashTable *adapters;
  GHashTable *devices;
  BluezBatteryCache *battery;
};

static gint
bluez_object_store_string_compare(gconstpointer left, gconstpointer right)
{
  return g_strcmp0(left, right);
}

static char **
bluez_object_store_dup_sorted_hash_keys(GHashTable *table)
{
  GList *keys = table ? g_hash_table_get_keys(table) : NULL;
  guint length = g_list_length(keys);
  char **paths = g_new0(char *, length + 1);
  guint index = 0;

  keys = g_list_sort(keys, bluez_object_store_string_compare);
  for (GList *node = keys; node; node = node->next)
    paths[index++] = g_strdup(node->data);

  g_list_free(keys);
  return paths;
}

BluezObjectStore *
bluez_object_store_new(void)
{
  BluezObjectStore *store = g_new0(BluezObjectStore, 1);

  store->adapters = g_hash_table_new_full(g_str_hash,
                                          g_str_equal,
                                          g_free,
                                          (GDestroyNotify)bluez_properties_free);
  store->devices = g_hash_table_new_full(g_str_hash,
                                         g_str_equal,
                                         g_free,
                                         (GDestroyNotify)bluez_properties_free);
  store->battery = bluez_battery_cache_new();
  return store;
}

void
bluez_object_store_free(BluezObjectStore *store)
{
  if (!store)
    return;

  g_hash_table_destroy(store->adapters);
  g_hash_table_destroy(store->devices);
  bluez_battery_cache_free(store->battery);
  g_free(store);
}

void
bluez_object_store_clear(BluezObjectStore *store)
{
  if (!store)
    return;

  g_hash_table_remove_all(store->adapters);
  g_hash_table_remove_all(store->devices);
  bluez_battery_cache_clear(store->battery);
}

gboolean
bluez_object_store_path_is_valid(const char *path)
{
  return bluez_object_path_is_valid(path);
}

gboolean
bluez_object_store_contains_adapter(BluezObjectStore *store, const char *path)
{
  return store && bluez_object_store_path_is_valid(path) &&
         g_hash_table_contains(store->adapters, path);
}

char **
bluez_object_store_dup_adapter_paths_sorted(BluezObjectStore *store)
{
  return bluez_object_store_dup_sorted_hash_keys(store ? store->adapters : NULL);
}

char **
bluez_object_store_dup_device_paths_sorted(BluezObjectStore *store)
{
  return bluez_object_store_dup_sorted_hash_keys(store ? store->devices : NULL);
}

void
bluez_object_store_replace_adapter(BluezObjectStore *store,
                                   const char *path,
                                   BluezProperties *properties)
{
  if (!store || !bluez_object_store_path_is_valid(path) || !properties) {
    bluez_properties_free(properties);
    return;
  }

  g_hash_table_replace(store->adapters, g_strdup(path), properties);
}

void
bluez_object_store_replace_device(BluezObjectStore *store,
                                  const char *path,
                                  BluezProperties *properties)
{
  if (!store || !bluez_object_store_path_is_valid(path) || !properties) {
    bluez_properties_free(properties);
    return;
  }

  g_hash_table_replace(store->devices, g_strdup(path), properties);
}

gboolean
bluez_object_store_remove_adapter(BluezObjectStore *store, const char *path)
{
  if (!store || !bluez_object_store_path_is_valid(path))
    return FALSE;

  return g_hash_table_remove(store->adapters, path);
}

void
bluez_object_store_remove_device(BluezObjectStore *store, const char *path)
{
  if (store && bluez_object_store_path_is_valid(path))
    g_hash_table_remove(store->devices, path);
}

void
bluez_object_store_set_device_battery(BluezObjectStore *store,
                                      const char *path,
                                      int percentage)
{
  if (store && bluez_object_store_path_is_valid(path))
    bluez_battery_cache_set(store->battery, path, percentage);
}

void
bluez_object_store_remove_device_battery(BluezObjectStore *store, const char *path)
{
  if (store && bluez_object_store_path_is_valid(path))
    bluez_battery_cache_remove(store->battery, path);
}

int
bluez_object_store_device_battery_value(BluezObjectStore *store,
                                        const char *path,
                                        gboolean *found)
{
  gboolean has_value = FALSE;
  int battery = 0;

  if (store && bluez_object_store_path_is_valid(path))
    battery = bluez_battery_cache_get(store->battery, path, &has_value);

  if (found)
    *found = has_value;

  return has_value ? battery : 0;
}

BluezProperties *
bluez_object_store_adapter(BluezObjectStore *store, const char *path)
{
  if (!store || !bluez_object_store_path_is_valid(path))
    return NULL;

  return g_hash_table_lookup(store->adapters, path);
}

BluezProperties *
bluez_object_store_device(BluezObjectStore *store, const char *path)
{
  if (!store || !bluez_object_store_path_is_valid(path))
    return NULL;

  return g_hash_table_lookup(store->devices, path);
}
