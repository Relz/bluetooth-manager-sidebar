#include "obex/obex-incoming-authorization-acceptance.h"

#include "obex/obex-dbus.h"
#include "obex/obex-incoming-notifications.h"
#include "obex/obex-incoming-transfer.h"
#include "obex/obex-service-registry-private.h"

static void
obex_incoming_authorization_return_staging_path(GDBusMethodInvocation *invocation,
                                                ObexIncomingTransfer *transfer)
{
  g_dbus_method_invocation_return_value(
    invocation,
    g_variant_new("(s)", obex_incoming_transfer_staging_path(transfer)));
}

static void
obex_incoming_authorization_reject_for_storage_error(ObexService *service,
                                                     GDBusMethodInvocation *invocation,
                                                     const GError *error)
{
  g_autofree char *message = obex_incoming_notifications_notify_location_error(
    service,
    error ? error->message : NULL);

  g_dbus_method_invocation_return_dbus_error(invocation, OBEX_ERROR_REJECTED, message);
}

static ObexIncomingTransfer *
obex_incoming_authorization_prepare_registered_transfer(ObexService *service,
                                                        ObexTransferDetails *details,
                                                        GError **error)
{
  ObexIncomingTransfer *transfer = obex_incoming_transfer_prepare(service,
                                                                     details,
                                                                     error);

  if (transfer)
    obex_service_store_incoming_transfer(service, details->path, transfer);
  return transfer;
}

static void
obex_incoming_authorization_report_receiving(ObexService *service,
                                             ObexIncomingTransfer *transfer)
{
  obex_incoming_notifications_notify_receiving(service, transfer);
}

gboolean
obex_incoming_authorization_accept_transfer(ObexService *service,
                                            ObexTransferDetails *details,
                                            GDBusMethodInvocation *invocation)
{
  g_autoptr(GError) error = NULL;
  ObexIncomingTransfer *transfer = obex_incoming_authorization_prepare_registered_transfer(service,
                                                                                              details,
                                                                                              &error);

  if (!transfer) {
    obex_incoming_authorization_reject_for_storage_error(service, invocation, error);
    return FALSE;
  }

  obex_incoming_authorization_report_receiving(service, transfer);
  obex_incoming_authorization_return_staging_path(invocation, transfer);
  return TRUE;
}
