#include "bluez/bluez-object-manager.h"

#include "bluez/bluez-object-manager-signals.h"

#define BLUEZ_BUS_NAME "org.bluez"
#define BLUEZ_OBJECT_MANAGER_PATH "/"
#define DBUS_OBJECT_MANAGER_INTERFACE "org.freedesktop.DBus.ObjectManager"
#define DBUS_PROPERTIES_INTERFACE "org.freedesktop.DBus.Properties"
#define BLUEZ_GET_MANAGED_OBJECTS_TIMEOUT_MS 2000

struct _BluezObjectManager {
  GDBusConnection *connection;
  guint interfaces_added_signal_id;
  guint interfaces_removed_signal_id;
  guint properties_changed_signal_id;
  BluezObjectManagerSignalCallbacks signal_callbacks;
};

static void
bluez_object_manager_on_interfaces_added(GDBusConnection *connection,
                                             const char *sender_name,
                                             const char *object_path,
                                             const char *interface_name,
                                             const char *signal_name,
                                             GVariant *parameters,
                                             gpointer user_data)
{
  BluezObjectManager *manager = user_data;

  (void)connection;
  (void)sender_name;
  (void)object_path;
  (void)interface_name;
  (void)signal_name;

  bluez_object_manager_signals_dispatch_interfaces_added(&manager->signal_callbacks,
                                                         parameters);
}

static void
bluez_object_manager_on_interfaces_removed(GDBusConnection *connection,
                                               const char *sender_name,
                                               const char *object_path,
                                               const char *interface_name,
                                               const char *signal_name,
                                               GVariant *parameters,
                                               gpointer user_data)
{
  BluezObjectManager *manager = user_data;

  (void)connection;
  (void)sender_name;
  (void)object_path;
  (void)interface_name;
  (void)signal_name;

  bluez_object_manager_signals_dispatch_interfaces_removed(&manager->signal_callbacks,
                                                           parameters);
}

static void
bluez_object_manager_on_properties_changed(GDBusConnection *connection,
                                               const char *sender_name,
                                               const char *object_path,
                                               const char *interface_name,
                                               const char *signal_name,
                                               GVariant *parameters,
                                               gpointer user_data)
{
  BluezObjectManager *manager = user_data;

  (void)connection;
  (void)sender_name;
  (void)interface_name;
  (void)signal_name;

  bluez_object_manager_signals_dispatch_properties_changed(&manager->signal_callbacks,
                                                           object_path,
                                                           parameters);
}

BluezObjectManager *
bluez_object_manager_new_system(GError **error)
{
  BluezObjectManager *manager = g_new0(BluezObjectManager, 1);

  manager->connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, error);
  if (!manager->connection) {
    g_free(manager);
    return NULL;
  }

  return manager;
}

void
bluez_object_manager_free(BluezObjectManager *manager)
{
  if (!manager)
    return;

  bluez_object_manager_close(manager);
  g_clear_object(&manager->connection);
  g_free(manager);
}

GDBusConnection *
bluez_object_manager_connection(BluezObjectManager *manager)
{
  return manager ? manager->connection : NULL;
}

GVariant *
bluez_object_manager_get_managed_objects(BluezObjectManager *manager,
                                             GError **error)
{
  g_autoptr(GVariant) reply = NULL;

  g_return_val_if_fail(manager != NULL, NULL);

  reply = g_dbus_connection_call_sync(manager->connection,
                                      BLUEZ_BUS_NAME,
                                      BLUEZ_OBJECT_MANAGER_PATH,
                                      DBUS_OBJECT_MANAGER_INTERFACE,
                                      "GetManagedObjects",
                                      NULL,
                                      G_VARIANT_TYPE("(a{oa{sa{sv}}})"),
                                      G_DBUS_CALL_FLAGS_NONE,
                                      BLUEZ_GET_MANAGED_OBJECTS_TIMEOUT_MS,
                                      NULL,
                                      error);
  if (!reply)
    return NULL;

  return g_variant_get_child_value(reply, 0);
}

void
bluez_object_manager_start(BluezObjectManager *manager,
                               BluezObjectManagerInterfacesAdded on_interfaces_added,
                               BluezObjectManagerInterfacesRemoved on_interfaces_removed,
                               BluezObjectManagerPropertiesChanged on_properties_changed,
                               gpointer user_data)
{
  g_return_if_fail(manager != NULL);

  bluez_object_manager_close(manager);

  manager->signal_callbacks = (BluezObjectManagerSignalCallbacks){
    .on_interfaces_added = on_interfaces_added,
    .on_interfaces_removed = on_interfaces_removed,
    .on_properties_changed = on_properties_changed,
    .user_data = user_data,
  };

  manager->interfaces_added_signal_id =
    g_dbus_connection_signal_subscribe(manager->connection,
                                       BLUEZ_BUS_NAME,
                                       DBUS_OBJECT_MANAGER_INTERFACE,
                                       "InterfacesAdded",
                                       BLUEZ_OBJECT_MANAGER_PATH,
                                       NULL,
                                       G_DBUS_SIGNAL_FLAGS_NONE,
                                       bluez_object_manager_on_interfaces_added,
                                       manager,
                                       NULL);
  manager->interfaces_removed_signal_id =
    g_dbus_connection_signal_subscribe(manager->connection,
                                       BLUEZ_BUS_NAME,
                                       DBUS_OBJECT_MANAGER_INTERFACE,
                                       "InterfacesRemoved",
                                       BLUEZ_OBJECT_MANAGER_PATH,
                                       NULL,
                                       G_DBUS_SIGNAL_FLAGS_NONE,
                                       bluez_object_manager_on_interfaces_removed,
                                       manager,
                                       NULL);
  manager->properties_changed_signal_id =
    g_dbus_connection_signal_subscribe(manager->connection,
                                       BLUEZ_BUS_NAME,
                                       DBUS_PROPERTIES_INTERFACE,
                                       "PropertiesChanged",
                                       NULL,
                                       NULL,
                                       G_DBUS_SIGNAL_FLAGS_NONE,
                                       bluez_object_manager_on_properties_changed,
                                       manager,
                                       NULL);
}

void
bluez_object_manager_close(BluezObjectManager *manager)
{
  if (!manager)
    return;

  if (manager->interfaces_added_signal_id) {
    g_dbus_connection_signal_unsubscribe(manager->connection,
                                         manager->interfaces_added_signal_id);
    manager->interfaces_added_signal_id = 0;
  }

  if (manager->interfaces_removed_signal_id) {
    g_dbus_connection_signal_unsubscribe(manager->connection,
                                         manager->interfaces_removed_signal_id);
    manager->interfaces_removed_signal_id = 0;
  }

  if (manager->properties_changed_signal_id) {
    g_dbus_connection_signal_unsubscribe(manager->connection,
                                         manager->properties_changed_signal_id);
    manager->properties_changed_signal_id = 0;
  }

  manager->signal_callbacks = (BluezObjectManagerSignalCallbacks){ 0 };
}
