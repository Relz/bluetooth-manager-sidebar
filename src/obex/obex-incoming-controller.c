#include "obex/obex-incoming-controller.h"

#include "obex/obex-dbus.h"
#include "obex/obex-incoming-authorization-acceptance.h"
#include "obex/obex-incoming-notifications.h"
#include "obex/obex-incoming-transfer.h"
#include "obex/obex-service-authorization-private.h"
#include "obex/obex-service-registry-private.h"

void
obex_incoming_reject_active(ObexService *service, const char *error_name, const char *message)
{
  ObexPendingAuthorization *request = obex_service_steal_active_authorization(service);

  if (!request)
    return;

  obex_incoming_notifications_withdraw(service);
  g_dbus_method_invocation_return_dbus_error(obex_pending_authorization_invocation(request),
                                             error_name,
                                             message);
  obex_pending_authorization_free(request);
}

void
obex_incoming_accept_active(ObexService *service)
{
  ObexPendingAuthorization *request = obex_service_steal_active_authorization(service);

  if (!request)
    return;

  obex_incoming_notifications_withdraw(service);
  obex_incoming_authorization_accept_transfer(service,
                                              obex_pending_authorization_details(request),
                                              obex_pending_authorization_invocation(request));
  obex_pending_authorization_free(request);
}

void
obex_incoming_cleanup_transfer_foreach(gpointer key, gpointer value, gpointer user_data)
{
  (void)key;
  (void)user_data;
  obex_incoming_transfer_cleanup(value);
}

static void
obex_incoming_notify_transfer_settlement(ObexService *service,
                                         const ObexIncomingTransferSettlement *settlement)
{
  if (!settlement)
    return;

  switch (settlement->status) {
    case OBEX_INCOMING_TRANSFER_STATUS_SAVED:
      obex_incoming_notifications_notify_saved(service, settlement->final_path);
      break;
    case OBEX_INCOMING_TRANSFER_STATUS_SAVE_ERROR:
      obex_incoming_notifications_notify_save_error(service, settlement->error_message);
      break;
    case OBEX_INCOMING_TRANSFER_STATUS_FAILED:
      obex_incoming_notifications_notify_failed(service);
      break;
    case OBEX_INCOMING_TRANSFER_STATUS_PENDING:
    default:
      break;
  }
}

gboolean
obex_incoming_handle_transfer_status(ObexService *service,
                                      const char *transfer_path,
                                      const char *status)
{
  ObexIncomingTransfer *incoming = NULL;
  ObexIncomingTransferSettlement settlement = { 0 };

  if (!status || !*status)
    return FALSE;

  incoming = obex_service_lookup_incoming_transfer(service, transfer_path);
  if (!incoming)
    return FALSE;

  if (!obex_incoming_transfer_settle_status(incoming, status, &settlement))
    return FALSE;

  obex_service_remove_incoming_transfer(service, transfer_path);
  obex_incoming_notify_transfer_settlement(service, &settlement);
  obex_incoming_transfer_settlement_clear(&settlement);

  return TRUE;
}
