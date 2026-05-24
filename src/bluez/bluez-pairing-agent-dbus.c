#include "bluez/bluez-pairing-agent-private.h"

static void
bluez_pairing_agent_on_method_call(GDBusConnection *connection,
                                   const char *sender,
                                   const char *object_path,
                                   const char *interface_name,
                                   const char *method_name,
                                   GVariant *parameters,
                                   GDBusMethodInvocation *invocation,
                                   gpointer user_data)
{
  BluezPairingAgent *agent = user_data;

  (void)connection;
  (void)sender;
  (void)object_path;
  (void)interface_name;

  if (g_str_equal(method_name, "Release")) {
    bluez_pairing_agent_close_active(agent, TRUE, PAIRING_RELEASED_MESSAGE);
    g_dbus_method_invocation_return_value(invocation, NULL);
  } else if (g_str_equal(method_name, "Cancel")) {
    bluez_pairing_agent_close_active(agent, TRUE, PAIRING_CANCELLED_MESSAGE);
    g_dbus_method_invocation_return_value(invocation, NULL);
  } else if (g_str_equal(method_name, "RequestPinCode")) {
    const char *device_path = NULL;

    g_variant_get(parameters, "(&o)", &device_path);
    bluez_pairing_agent_request_pin_code(agent, device_path, invocation);
  } else if (g_str_equal(method_name, "DisplayPinCode")) {
    const char *device_path = NULL;
    const char *pin_code = NULL;

    g_variant_get(parameters, "(&o&s)", &device_path, &pin_code);
    bluez_pairing_agent_display_pin_code(agent, device_path, pin_code);
    g_dbus_method_invocation_return_value(invocation, NULL);
  } else if (g_str_equal(method_name, "RequestPasskey")) {
    const char *device_path = NULL;

    g_variant_get(parameters, "(&o)", &device_path);
    bluez_pairing_agent_request_passkey(agent, device_path, invocation);
  } else if (g_str_equal(method_name, "DisplayPasskey")) {
    const char *device_path = NULL;
    guint32 passkey = 0;
    guint16 entered = 0;

    g_variant_get(parameters, "(&ouq)", &device_path, &passkey, &entered);
    bluez_pairing_agent_display_passkey(agent, device_path, passkey, entered);
    g_dbus_method_invocation_return_value(invocation, NULL);
  } else if (g_str_equal(method_name, "RequestConfirmation")) {
    const char *device_path = NULL;
    guint32 passkey = 0;

    g_variant_get(parameters, "(&ou)", &device_path, &passkey);
    bluez_pairing_agent_request_confirmation(agent, device_path, passkey, invocation);
  } else if (g_str_equal(method_name, "RequestAuthorization")) {
    const char *device_path = NULL;

    g_variant_get(parameters, "(&o)", &device_path);
    bluez_pairing_agent_request_authorization(agent, device_path, invocation);
  } else if (g_str_equal(method_name, "AuthorizeService")) {
    const char *device_path = NULL;
    const char *uuid = NULL;

    g_variant_get(parameters, "(&o&s)", &device_path, &uuid);
    bluez_pairing_agent_authorize_service(agent, device_path, uuid, invocation);
  } else {
    g_autofree char *message = g_strdup_printf("Unsupported method %s", method_name);

    g_dbus_method_invocation_return_dbus_error(invocation,
                                               BLUEZ_PAIRING_ERROR_NOT_SUPPORTED,
                                               message);
  }
}

static const GDBusInterfaceVTable bluez_pairing_agent_vtable = {
  .method_call = bluez_pairing_agent_on_method_call,
};

const GDBusInterfaceVTable *
bluez_pairing_agent_dbus_vtable(void)
{
  return &bluez_pairing_agent_vtable;
}
