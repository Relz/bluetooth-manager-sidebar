#include "obex/obex-agent-lifecycle.h"

struct _ObexAgentLifecycle {
  GDBusConnection *connection;
  GDBusNodeInfo *node_info;
  guint registration_id;
  gboolean registered_with_obex;
};

ObexAgentLifecycle *
obex_agent_lifecycle_new(void)
{
  return g_new0(ObexAgentLifecycle, 1);
}

void
obex_agent_lifecycle_free(ObexAgentLifecycle *lifecycle)
{
  if (!lifecycle)
    return;

  obex_agent_lifecycle_clear_node_info(lifecycle);
  obex_agent_lifecycle_clear_connection(lifecycle);
  g_free(lifecycle);
}

GDBusConnection *
obex_agent_lifecycle_connection(ObexAgentLifecycle *lifecycle)
{
  return lifecycle ? lifecycle->connection : NULL;
}

void
obex_agent_lifecycle_take_connection(ObexAgentLifecycle *lifecycle,
                                     GDBusConnection *connection)
{
  if (!lifecycle) {
    g_clear_object(&connection);
    return;
  }

  g_clear_object(&lifecycle->connection);
  lifecycle->connection = connection;
}

void
obex_agent_lifecycle_clear_connection(ObexAgentLifecycle *lifecycle)
{
  if (lifecycle)
    g_clear_object(&lifecycle->connection);
}

GDBusNodeInfo *
obex_agent_lifecycle_node_info(ObexAgentLifecycle *lifecycle)
{
  return lifecycle ? lifecycle->node_info : NULL;
}

void
obex_agent_lifecycle_take_node_info(ObexAgentLifecycle *lifecycle,
                                    GDBusNodeInfo *node_info)
{
  if (!lifecycle) {
    if (node_info)
      g_dbus_node_info_unref(node_info);
    return;
  }

  g_clear_pointer(&lifecycle->node_info, g_dbus_node_info_unref);
  lifecycle->node_info = node_info;
}

void
obex_agent_lifecycle_clear_node_info(ObexAgentLifecycle *lifecycle)
{
  if (lifecycle)
    g_clear_pointer(&lifecycle->node_info, g_dbus_node_info_unref);
}

guint
obex_agent_lifecycle_registration_id(ObexAgentLifecycle *lifecycle)
{
  return lifecycle ? lifecycle->registration_id : 0;
}

void
obex_agent_lifecycle_set_registration_id(ObexAgentLifecycle *lifecycle,
                                         guint registration_id)
{
  if (lifecycle)
    lifecycle->registration_id = registration_id;
}

gboolean
obex_agent_lifecycle_registered_with_obex(ObexAgentLifecycle *lifecycle)
{
  return lifecycle && lifecycle->registered_with_obex;
}

void
obex_agent_lifecycle_set_registered_with_obex(ObexAgentLifecycle *lifecycle,
                                              gboolean registered)
{
  if (lifecycle)
    lifecycle->registered_with_obex = registered;
}
