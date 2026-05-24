#pragma once

#include <gio/gio.h>

typedef struct _BluezObjectManager BluezObjectManager;

/* GVariant callback arguments are borrowed and valid only during the callback. */
typedef void (*BluezObjectManagerInterfacesAdded)(const char *path,
                                                     GVariant *interfaces,
                                                     gpointer user_data);
typedef void (*BluezObjectManagerInterfacesRemoved)(const char *path,
                                                       GVariant *interfaces,
                                                       gpointer user_data);
typedef void (*BluezObjectManagerPropertiesChanged)(const char *object_path,
                                                       const char *changed_interface,
                                                       GVariant *changed,
                                                       GVariant *invalidated,
                                                       gpointer user_data);

BluezObjectManager *bluez_object_manager_new_system(GError **error);
void bluez_object_manager_free(BluezObjectManager *manager);
/* Borrowed connection. */
GDBusConnection *bluez_object_manager_connection(BluezObjectManager *manager);
/* Returns an owned a{oa{sa{sv}}} variant, not the D-Bus reply tuple. */
GVariant *bluez_object_manager_get_managed_objects(BluezObjectManager *manager,
                                                       GError **error);
void bluez_object_manager_start(BluezObjectManager *manager,
                                    BluezObjectManagerInterfacesAdded on_interfaces_added,
                                    BluezObjectManagerInterfacesRemoved on_interfaces_removed,
                                    BluezObjectManagerPropertiesChanged on_properties_changed,
                                    gpointer user_data);
void bluez_object_manager_close(BluezObjectManager *manager);
