#include "bluez/bluez-pairing-agent-contract.h"

#define BLUEZ_AGENT_OBJECT_PATH "/dev/relz/BmSidebar/Agent"
#define BLUEZ_AGENT_CAPABILITY "KeyboardDisplay"

static const char *BLUEZ_AGENT_INTROSPECTION_XML =
  "<node>\n"
  "  <interface name=\"org.bluez.Agent1\">\n"
  "    <method name=\"Release\"/>\n"
  "    <method name=\"RequestPinCode\">\n"
  "      <arg name=\"device\" type=\"o\" direction=\"in\"/>\n"
  "      <arg name=\"pincode\" type=\"s\" direction=\"out\"/>\n"
  "    </method>\n"
  "    <method name=\"DisplayPinCode\">\n"
  "      <arg name=\"device\" type=\"o\" direction=\"in\"/>\n"
  "      <arg name=\"pincode\" type=\"s\" direction=\"in\"/>\n"
  "    </method>\n"
  "    <method name=\"RequestPasskey\">\n"
  "      <arg name=\"device\" type=\"o\" direction=\"in\"/>\n"
  "      <arg name=\"passkey\" type=\"u\" direction=\"out\"/>\n"
  "    </method>\n"
  "    <method name=\"DisplayPasskey\">\n"
  "      <arg name=\"device\" type=\"o\" direction=\"in\"/>\n"
  "      <arg name=\"passkey\" type=\"u\" direction=\"in\"/>\n"
  "      <arg name=\"entered\" type=\"q\" direction=\"in\"/>\n"
  "    </method>\n"
  "    <method name=\"RequestConfirmation\">\n"
  "      <arg name=\"device\" type=\"o\" direction=\"in\"/>\n"
  "      <arg name=\"passkey\" type=\"u\" direction=\"in\"/>\n"
  "    </method>\n"
  "    <method name=\"RequestAuthorization\">\n"
  "      <arg name=\"device\" type=\"o\" direction=\"in\"/>\n"
  "    </method>\n"
  "    <method name=\"AuthorizeService\">\n"
  "      <arg name=\"device\" type=\"o\" direction=\"in\"/>\n"
  "      <arg name=\"uuid\" type=\"s\" direction=\"in\"/>\n"
  "    </method>\n"
  "    <method name=\"Cancel\"/>\n"
  "  </interface>\n"
  "</node>\n";

const char *
bluez_pairing_agent_contract_object_path(void)
{
  return BLUEZ_AGENT_OBJECT_PATH;
}

const char *
bluez_pairing_agent_contract_capability(void)
{
  return BLUEZ_AGENT_CAPABILITY;
}

GDBusNodeInfo *
bluez_pairing_agent_contract_node_info_new(GError **error)
{
  return g_dbus_node_info_new_for_xml(BLUEZ_AGENT_INTROSPECTION_XML, error);
}
