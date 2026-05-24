#include "obex/obex-outgoing-status.h"

#include "obex/obex-outgoing-notifications.h"
#include "obex/obex-sender.h"
#include "obex/obex-service-registry-private.h"
#include "obex/obex-service-session-private.h"

ObexOutgoingTransferStatus
obex_outgoing_transfer_status_from_text(const char *status)
{
  if (g_strcmp0(status, "complete") == 0)
    return OBEX_OUTGOING_TRANSFER_STATUS_COMPLETE;
  if (g_strcmp0(status, "error") == 0)
    return OBEX_OUTGOING_TRANSFER_STATUS_ERROR;
  return OBEX_OUTGOING_TRANSFER_STATUS_PENDING;
}

static void
obex_outgoing_notify_transfer_status(ObexService *service,
                                     const char *file_name,
                                     const char *device_name,
                                     ObexOutgoingTransferStatus status)
{
  if (status == OBEX_OUTGOING_TRANSFER_STATUS_COMPLETE)
    obex_outgoing_notifications_notify_sent(service, file_name);
  else if (status == OBEX_OUTGOING_TRANSFER_STATUS_ERROR) {
    obex_outgoing_notifications_notify_failed(service);
    obex_outgoing_notifications_notify_rejected(service, device_name, file_name);
  }
}

static void
obex_outgoing_remove_settled_transfer(ObexService *service,
                                      const char *transfer_path,
                                      const char *session_path)
{
  if (session_path)
    obex_service_remove_session(service, session_path);
  if (transfer_path)
    obex_service_remove_outgoing_transfer(service, transfer_path);
}

gboolean
obex_outgoing_settle_transfer_status(ObexService *service,
                                      const char *transfer_path,
                                      const char *session_path,
                                      const char *file_name,
                                      const char *device_name,
                                      const char *status)
{
  ObexOutgoingTransferStatus transfer_status;

  if (!status || !*status)
    return FALSE;

  transfer_status = obex_outgoing_transfer_status_from_text(status);
  if (transfer_status == OBEX_OUTGOING_TRANSFER_STATUS_PENDING)
    return FALSE;

  obex_outgoing_notify_transfer_status(service, file_name, device_name, transfer_status);
  obex_outgoing_remove_settled_transfer(service, transfer_path, session_path);
  return TRUE;
}

gboolean
obex_outgoing_handle_transfer_status(ObexService *service,
                                     const char *transfer_path,
                                     const char *status)
{
  ObexOutgoingTransfer *outgoing = NULL;

  if (!status || !*status)
    return FALSE;

  outgoing = obex_service_lookup_outgoing_transfer(service, transfer_path);
  if (!outgoing)
    return FALSE;

  return obex_outgoing_settle_transfer_status(service,
                                              transfer_path,
                                              obex_outgoing_transfer_session_path(outgoing),
                                              obex_outgoing_transfer_file_name(outgoing),
                                              obex_outgoing_transfer_device_name(outgoing),
                                              status);
}

void
obex_outgoing_remove_session_foreach(gpointer key, gpointer value, gpointer user_data)
{
  ObexService *service = user_data;
  ObexOutgoingTransfer *transfer = value;

  (void)key;
  obex_service_remove_session(service, obex_outgoing_transfer_session_path(transfer));
}
