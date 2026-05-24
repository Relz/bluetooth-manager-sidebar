#include "bluez/bluez-cache-updater.h"

#include "bluez/bluez-cache-applier.h"
#include "bluez/bluez-cache-selection.h"

void
bluez_cache_updater_replace_from_managed_objects(BluezObjectCache *cache,
                                                 GVariant *managed_objects)
{
  if (!cache)
    return;

  if (managed_objects && g_variant_is_of_type(managed_objects, G_VARIANT_TYPE("a{oa{sa{sv}}}"))) {
    for (gsize index = 0; index < g_variant_n_children(managed_objects); index++) {
      g_autoptr(GVariant) entry = g_variant_get_child_value(managed_objects, index);
      g_autoptr(GVariant) key = g_variant_get_child_value(entry, 0);
      g_autoptr(GVariant) interfaces = g_variant_get_child_value(entry, 1);
      const char *path = g_variant_get_string(key, NULL);

      bluez_cache_applier_cache_interfaces(cache, path, interfaces);
    }
  }

  bluez_cache_selection_refresh_primary(cache, TRUE);
}

void
bluez_cache_updater_cache_interfaces(BluezObjectCache *cache,
                                     const char *path,
                                     GVariant *interfaces)
{
  bluez_cache_selection_refresh_primary(cache,
                                        bluez_cache_applier_cache_interfaces(cache,
                                                                            path,
                                                                            interfaces));
}

void
bluez_cache_updater_remove_interfaces(BluezObjectCache *cache,
                                      const char *path,
                                      GVariant *interfaces)
{
  bluez_cache_selection_refresh_primary(cache,
                                        bluez_cache_applier_remove_interfaces(cache,
                                                                             path,
                                                                             interfaces));
}

void
bluez_cache_updater_apply_properties_changed(BluezObjectCache *cache,
                                             const char *object_path,
                                             const char *changed_interface,
                                             GVariant *changed,
                                             GVariant *invalidated)
{
  bluez_cache_applier_apply_properties_changed(cache,
                                               object_path,
                                               changed_interface,
                                               changed,
                                               invalidated);
}
