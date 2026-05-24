#include "obex/obex-service-lifecycle-steps.h"

#include "obex/obex-agent-handler.h"
#include "obex/obex-dbus.h"
#include "obex/obex-incoming-controller.h"
#include "obex/obex-service-agent-private.h"
#include "obex/obex-service-monitor-private.h"
#include "obex/obex-service-notification-private.h"
#include "obex/obex-service-registry-private.h"

gboolean
obex_service_lifecycle_connect_session_bus(ObexService *service, GError **error)
{
  obex_service_agent_take_connection(service, g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, error));
  return obex_service_connection(service) != NULL;
}

gboolean
obex_service_lifecycle_register_agent_object(ObexService *service, GError **error)
{
  return obex_agent_handler_register_object(service, error);
}

void
obex_service_lifecycle_start_transfer_monitor(ObexService *service,
                                              ObexTransferStatusHandler handler,
                                              gpointer user_data)
{
  GDBusConnection *connection = obex_service_connection(service);

  if (connection)
    obex_service_take_transfer_monitor(service,
                                       obex_transfer_monitor_new(connection, handler, user_data));
}

gboolean
obex_service_lifecycle_register_agent_with_obex(ObexService *service, GError **error)
{
  return obex_agent_handler_register_with_obex(service, error);
}

void
obex_service_lifecycle_cancel_incoming(ObexService *service)
{
  obex_incoming_reject_active(service, OBEX_ERROR_CANCELED, "Bluetooth file transfer was cancelled");
}

void
obex_service_lifecycle_withdraw_notifications(ObexService *service)
{
  obex_service_withdraw_notification(service, OBEX_INCOMING_NOTIFICATION_ID);
  obex_service_withdraw_notification(service, OBEX_COMPLETED_NOTIFICATION_ID);
  obex_service_withdraw_notification(service, OBEX_OUTGOING_FAILED_NOTIFICATION_ID);
}

void
obex_service_lifecycle_unregister_agent(ObexService *service)
{
  obex_agent_handler_unregister_from_obex(service);
  obex_agent_handler_unregister_object(service);
}

void
obex_service_lifecycle_stop_transfer_monitor(ObexService *service)
{
  obex_service_clear_transfer_monitor(service);
}

void
obex_service_lifecycle_clear_transfers(ObexService *service)
{
  obex_service_transfer_registry_shutdown(service);
}

void
obex_service_lifecycle_clear_agent_state(ObexService *service)
{
  obex_service_agent_clear_node_info(service);
  obex_service_agent_clear_connection(service);
}
