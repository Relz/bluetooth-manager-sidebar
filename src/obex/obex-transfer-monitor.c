#include "obex/obex-transfer-monitor.h"

#include "obex/obex-dbus.h"
#include "obex/obex-transfer-details.h"

struct _ObexTransferMonitor {
  GDBusConnection *connection;
  guint signal_id;
  ObexTransferStatusHandler handler;
  gpointer user_data;
};

static void
obex_transfer_monitor_signal(GDBusConnection *connection,
                             const char *sender_name,
                             const char *object_path,
                             const char *interface_name,
                             const char *signal_name,
                             GVariant *parameters,
                             gpointer user_data)
{
  ObexTransferMonitor *monitor = user_data;
  const char *changed_interface = NULL;
  g_autoptr(GVariant) changed = NULL;
  g_autoptr(GVariant) invalidated = NULL;
  g_autofree char *status = NULL;

  (void)connection;
  (void)sender_name;
  (void)interface_name;
  (void)signal_name;
  (void)invalidated;

  g_variant_get(parameters, "(&s@a{sv}@as)", &changed_interface, &changed, &invalidated);
  if (!g_str_equal(changed_interface, OBEX_TRANSFER_INTERFACE))
    return;

  status = obex_transfer_status_from_properties(changed);
  if (monitor->handler)
    monitor->handler(object_path, status, monitor->user_data);
}

ObexTransferMonitor *
obex_transfer_monitor_new(GDBusConnection *connection,
                          ObexTransferStatusHandler handler,
                          gpointer user_data)
{
  ObexTransferMonitor *monitor = NULL;

  if (!connection || !handler)
    return NULL;

  monitor = g_new0(ObexTransferMonitor, 1);
  monitor->connection = g_object_ref(connection);
  monitor->handler = handler;
  monitor->user_data = user_data;
  monitor->signal_id = g_dbus_connection_signal_subscribe(connection,
                                                          OBEX_BUS_NAME,
                                                          DBUS_PROPERTIES_INTERFACE,
                                                          "PropertiesChanged",
                                                          NULL,
                                                          OBEX_TRANSFER_INTERFACE,
                                                          G_DBUS_SIGNAL_FLAGS_NONE,
                                                          obex_transfer_monitor_signal,
                                                          monitor,
                                                          NULL);
  return monitor;
}

void
obex_transfer_monitor_free(ObexTransferMonitor *monitor)
{
  if (!monitor)
    return;

  if (monitor->connection && monitor->signal_id)
    g_dbus_connection_signal_unsubscribe(monitor->connection, monitor->signal_id);
  g_clear_object(&monitor->connection);
  g_free(monitor);
}
