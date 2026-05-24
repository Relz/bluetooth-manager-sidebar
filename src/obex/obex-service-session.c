#include "obex/obex-dbus.h"
#include "obex/obex-service-agent-private.h"
#include "obex/obex-service-session-private.h"

void
obex_service_remove_session_on_connection(GDBusConnection *connection, const char *session_path)
{
  g_autoptr(GError) error = NULL;
  g_autoptr(GVariant) reply = NULL;

  if (!connection || !session_path || !*session_path)
    return;

  reply = g_dbus_connection_call_sync(connection,
                                      OBEX_BUS_NAME,
                                      OBEX_ROOT_PATH,
                                      OBEX_CLIENT_INTERFACE,
                                      "RemoveSession",
                                      g_variant_new("(o)", session_path),
                                      NULL,
                                      G_DBUS_CALL_FLAGS_NONE,
                                      OBEX_REMOVE_SESSION_TIMEOUT_MS,
                                      NULL,
                                      &error);
}

void
obex_service_remove_session(ObexService *service, const char *session_path)
{
  obex_service_remove_session_on_connection(obex_service_connection(service), session_path);
}
