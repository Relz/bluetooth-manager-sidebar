#include "bluez/bluez-cache-query.h"

#include <string.h>

#include "bluez/bluez-cache-private.h"

static gboolean
bluez_cache_query_path_is_descendant(const char *path, const char *parent)
{
  gsize parent_length = 0;

  if (!path || !parent)
    return FALSE;

  parent_length = strlen(parent);
  return g_str_has_prefix(path, parent) && path[parent_length] == '/';
}

char **
bluez_cache_query_dup_adapter_paths(BluezObjectCache *cache)
{
  return bluez_object_cache_dup_adapter_paths_sorted(cache);
}

char **
bluez_cache_query_dup_device_paths(BluezObjectCache *cache, const char *adapter_path)
{
  GPtrArray *paths;
  g_auto(GStrv) all_paths = NULL;

  if (!cache)
    return g_new0(char *, 1);

  if (!adapter_path || !*adapter_path)
    return bluez_object_cache_dup_device_paths_sorted(cache);

  paths = g_ptr_array_new_with_free_func(g_free);
  all_paths = bluez_object_cache_dup_device_paths_sorted(cache);

  for (guint i = 0; all_paths && all_paths[i]; i++) {
    const char *device_path = all_paths[i];

    if (bluez_cache_query_path_is_descendant(device_path, adapter_path))
      g_ptr_array_add(paths, g_strdup(device_path));
  }

  g_ptr_array_add(paths, NULL);
  return (char **)g_ptr_array_free(paths, FALSE);
}

int
bluez_cache_query_device_battery(BluezObjectCache *cache,
                                 const char *path,
                                 gboolean *found)
{
  return bluez_object_cache_device_battery_value(cache, path, found);
}

char *
bluez_cache_query_dup_device_adapter_path(BluezObjectCache *cache,
                                          const char *device_path)
{
  if (!cache || !bluez_object_cache_path_is_valid(device_path))
    return NULL;

  g_auto(GStrv) adapter_paths = bluez_object_cache_dup_adapter_paths_sorted(cache);

  for (guint i = 0; adapter_paths && adapter_paths[i]; i++) {
    const char *adapter_path = adapter_paths[i];

    if (bluez_cache_query_path_is_descendant(device_path, adapter_path))
      return g_strdup(adapter_path);
  }

  const char *slash = g_strrstr(device_path, "/");
  if (!slash)
    return NULL;

  g_autofree char *parent_path = g_strndup(device_path, (gsize)(slash - device_path));
  if (bluez_object_cache_contains_adapter(cache, parent_path))
    return g_strdup(parent_path);

  return NULL;
}
