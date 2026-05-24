#include "obex/obex-service-agent-private.h"

#include "obex/obex-agent-lifecycle.h"
#include "obex/obex-service-components.h"
#include "obex/obex-service-private.h"

static ObexAgentLifecycle *
obex_service_agent(ObexService *service)
{
  return obex_service_components_agent(obex_service_components(service));
}

GDBusConnection *
obex_service_connection(ObexService *service)
{
  return obex_agent_lifecycle_connection(obex_service_agent(service));
}

void
obex_service_agent_take_connection(ObexService *service, GDBusConnection *connection)
{
  obex_agent_lifecycle_take_connection(obex_service_agent(service), connection);
}

GDBusNodeInfo *
obex_service_agent_node_info(ObexService *service)
{
  return obex_agent_lifecycle_node_info(obex_service_agent(service));
}

void
obex_service_agent_take_node_info(ObexService *service, GDBusNodeInfo *node_info)
{
  obex_agent_lifecycle_take_node_info(obex_service_agent(service), node_info);
}

void
obex_service_agent_clear_node_info(ObexService *service)
{
  obex_agent_lifecycle_clear_node_info(obex_service_agent(service));
}

void
obex_service_agent_clear_connection(ObexService *service)
{
  obex_agent_lifecycle_clear_connection(obex_service_agent(service));
}

guint
obex_service_agent_registration_id(ObexService *service)
{
  return obex_agent_lifecycle_registration_id(obex_service_agent(service));
}

void
obex_service_agent_set_registration_id(ObexService *service, guint registration_id)
{
  obex_agent_lifecycle_set_registration_id(obex_service_agent(service), registration_id);
}

gboolean
obex_service_agent_registered_with_obex(ObexService *service)
{
  return obex_agent_lifecycle_registered_with_obex(obex_service_agent(service));
}

void
obex_service_agent_set_registered_with_obex(ObexService *service, gboolean registered)
{
  obex_agent_lifecycle_set_registered_with_obex(obex_service_agent(service), registered);
}
