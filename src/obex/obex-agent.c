#include "obex/obex-agent.h"

#define OBEX_AGENT_PATH "/dev/relz/BmSidebar/ObexAgent"

static const char *OBEX_AGENT_INTROSPECTION_XML =
  "<node>\n"
  "  <interface name=\"org.bluez.obex.Agent1\">\n"
  "    <method name=\"Release\"/>\n"
  "    <method name=\"AuthorizePush\">\n"
  "      <arg name=\"transfer\" type=\"o\" direction=\"in\"/>\n"
  "      <arg name=\"filename\" type=\"s\" direction=\"out\"/>\n"
  "    </method>\n"
  "    <method name=\"Cancel\"/>\n"
  "  </interface>\n"
  "</node>\n";

const char *
obex_agent_path(void)
{
  return OBEX_AGENT_PATH;
}

GDBusNodeInfo *
obex_agent_node_info_new(GError **error)
{
  return g_dbus_node_info_new_for_xml(OBEX_AGENT_INTROSPECTION_XML, error);
}
