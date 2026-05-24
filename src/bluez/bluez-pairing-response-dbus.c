#include "bluez/bluez-pairing-response-dbus.h"

void
bluez_pairing_response_dbus_discard_variant(GVariant *value)
{
  if (value)
    g_variant_unref(g_variant_ref_sink(value));
}

void
bluez_pairing_response_dbus_return_value(PairingResponse *response, GVariant *value)
{
  g_dbus_method_invocation_return_value(response->invocation, value);
}

void
bluez_pairing_response_dbus_return_rejected(PairingResponse *response, const char *message)
{
  g_dbus_method_invocation_return_dbus_error(response->invocation,
                                             BLUEZ_PAIRING_ERROR_REJECTED,
                                             message);
}
