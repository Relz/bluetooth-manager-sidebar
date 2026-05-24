#pragma once

#include <gio/gio.h>

GVariant *bluez_dbus_variant_ref_unboxed(GVariant *value);
GVariant *bluez_dbus_dict_lookup_child_value(GVariant *dict, const char *name);
gboolean bluez_dbus_string_array_contains(GVariant *array, const char *needle);
gboolean bluez_dbus_changed_dict_get_int(GVariant *changed,
                                         const char *name,
                                         gint *out_value);
