#include "bluez/bluez-object-manager-signals.h"

#include "bluez/bluez-object-path.h"

void
bluez_object_manager_signals_dispatch_interfaces_added(
  const BluezObjectManagerSignalCallbacks *callbacks,
  GVariant *parameters)
{
  if (!callbacks || !callbacks->on_interfaces_added ||
      !parameters || !g_variant_is_of_type(parameters, G_VARIANT_TYPE("(oa{sa{sv}})")))
    return;

  g_autoptr(GVariant) path_value = g_variant_get_child_value(parameters, 0);
  g_autoptr(GVariant) interfaces = g_variant_get_child_value(parameters, 1);
  const char *path = g_variant_get_string(path_value, NULL);

  if (bluez_object_path_is_valid(path))
    callbacks->on_interfaces_added(path, interfaces, callbacks->user_data);
}

void
bluez_object_manager_signals_dispatch_interfaces_removed(
  const BluezObjectManagerSignalCallbacks *callbacks,
  GVariant *parameters)
{
  if (!callbacks || !callbacks->on_interfaces_removed ||
      !parameters || !g_variant_is_of_type(parameters, G_VARIANT_TYPE("(oas)")))
    return;

  g_autoptr(GVariant) path_value = g_variant_get_child_value(parameters, 0);
  g_autoptr(GVariant) interfaces = g_variant_get_child_value(parameters, 1);
  const char *path = g_variant_get_string(path_value, NULL);

  if (bluez_object_path_is_valid(path))
    callbacks->on_interfaces_removed(path, interfaces, callbacks->user_data);
}

void
bluez_object_manager_signals_dispatch_properties_changed(
  const BluezObjectManagerSignalCallbacks *callbacks,
  const char *object_path,
  GVariant *parameters)
{
  if (!callbacks || !callbacks->on_properties_changed ||
      !bluez_object_path_is_valid(object_path) ||
      !parameters || !g_variant_is_of_type(parameters, G_VARIANT_TYPE("(sa{sv}as)")))
    return;

  g_autoptr(GVariant) changed_interface_value = g_variant_get_child_value(parameters, 0);
  g_autoptr(GVariant) changed = g_variant_get_child_value(parameters, 1);
  g_autoptr(GVariant) invalidated = g_variant_get_child_value(parameters, 2);
  const char *changed_interface = g_variant_get_string(changed_interface_value, NULL);

  callbacks->on_properties_changed(object_path,
                                   changed_interface,
                                   changed,
                                   invalidated,
                                   callbacks->user_data);
}
