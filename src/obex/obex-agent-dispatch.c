#include "obex/obex-agent-dispatch.h"

#include "obex/obex-dbus.h"
#include "obex/obex-incoming-controller.h"
#include "obex/obex-service-agent-private.h"

static void
obex_agent_method_call(GDBusConnection *connection,
                       const char *sender,
                       const char *object_path,
                       const char *interface_name,
                       const char *method_name,
                       GVariant *parameters,
                       GDBusMethodInvocation *invocation,
                       gpointer user_data)
{
  ObexService *service = user_data;

  (void)connection;
  (void)sender;
  (void)object_path;
  (void)interface_name;

  if (g_str_equal(method_name, "Release")) {
    obex_service_agent_set_registered_with_obex(service, FALSE);
    obex_incoming_reject_active(service, OBEX_ERROR_CANCELED, "Bluetooth file receive agent was released");
    g_dbus_method_invocation_return_value(invocation, NULL);
  } else if (g_str_equal(method_name, "Cancel")) {
    obex_incoming_reject_active(service, OBEX_ERROR_CANCELED, "Bluetooth file transfer was cancelled");
    g_dbus_method_invocation_return_value(invocation, NULL);
  } else if (g_str_equal(method_name, "AuthorizePush")) {
    const char *transfer_path = NULL;

    g_variant_get(parameters, "(&o)", &transfer_path);
    obex_incoming_authorize_push(service, transfer_path, invocation);
  } else {
    g_autofree char *message = g_strdup_printf("Unsupported method %s", method_name);

    g_dbus_method_invocation_return_dbus_error(invocation, OBEX_ERROR_REJECTED, message);
  }
}

static const GDBusInterfaceVTable obex_agent_vtable = {
  .method_call = obex_agent_method_call,
};

const GDBusInterfaceVTable *
obex_agent_dispatch_vtable(void)
{
  return &obex_agent_vtable;
}
