#include "obex/obex-agent-handler.h"

#include "obex/obex-agent.h"
#include "obex/obex-agent-dispatch.h"
#include "obex/obex-dbus.h"
#include "obex/obex-service-agent-private.h"
#include "obex/obex-service-private.h"

gboolean
obex_agent_handler_register_object(ObexService *service, GError **error)
{
  GDBusNodeInfo *node_info = obex_service_agent_node_info(service);
  guint registration_id;

  if (!node_info) {
    node_info = obex_agent_node_info_new(error);
    if (!node_info)
      return FALSE;
    obex_service_agent_take_node_info(service, node_info);
  }

  node_info = obex_service_agent_node_info(service);
  registration_id = g_dbus_connection_register_object(obex_service_connection(service),
                                                       obex_agent_path(),
                                                       node_info->interfaces[0],
                                                       obex_agent_dispatch_vtable(),
                                                       obex_service_ref(service),
                                                      (GDestroyNotify)obex_service_unref,
                                                      error);
  obex_service_agent_set_registration_id(service, registration_id);
  return registration_id != 0;
}

gboolean
obex_agent_handler_register_with_obex(ObexService *service, GError **error)
{
  g_autoptr(GVariant) reply = NULL;

  reply = g_dbus_connection_call_sync(obex_service_connection(service),
                                      OBEX_BUS_NAME,
                                      OBEX_ROOT_PATH,
                                      OBEX_AGENT_MANAGER_INTERFACE,
                                      "RegisterAgent",
                                      g_variant_new("(o)", obex_agent_path()),
                                      NULL,
                                      G_DBUS_CALL_FLAGS_NONE,
                                      OBEX_REGISTER_TIMEOUT_MS,
                                      NULL,
                                      error);
  obex_service_agent_set_registered_with_obex(service, reply != NULL);
  return obex_service_agent_registered_with_obex(service);
}

void
obex_agent_handler_unregister_from_obex(ObexService *service)
{
  g_autoptr(GError) error = NULL;
  g_autoptr(GVariant) reply = NULL;

  if (!obex_service_agent_registered_with_obex(service) || !obex_service_connection(service))
    return;

  reply = g_dbus_connection_call_sync(obex_service_connection(service),
                                      OBEX_BUS_NAME,
                                      OBEX_ROOT_PATH,
                                      OBEX_AGENT_MANAGER_INTERFACE,
                                      "UnregisterAgent",
                                      g_variant_new("(o)", obex_agent_path()),
                                      NULL,
                                      G_DBUS_CALL_FLAGS_NONE,
                                      OBEX_UNREGISTER_TIMEOUT_MS,
                                      NULL,
                                      &error);
  obex_service_agent_set_registered_with_obex(service, FALSE);
}

void
obex_agent_handler_unregister_object(ObexService *service)
{
  if (obex_service_agent_registration_id(service) && obex_service_connection(service)) {
    g_dbus_connection_unregister_object(obex_service_connection(service),
                                        obex_service_agent_registration_id(service));
    obex_service_agent_set_registration_id(service, 0);
  }
}
