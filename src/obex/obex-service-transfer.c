#include "obex/obex-service-transfer.h"

#include "obex/obex-incoming-controller.h"
#include "obex/obex-outgoing-controller.h"
#include "obex/obex-outgoing-status.h"
#include "obex/obex-dbus.h"
#include "obex/obex-service-authorization-private.h"

void
obex_service_respond_to_file_notification(ObexService *service,
                                           const char *transfer_path,
                                           gboolean receive)
{
  ObexPendingAuthorization *request = obex_service_active_authorization(service);

  if (!request || !transfer_path ||
      g_strcmp0(obex_pending_authorization_path(request), transfer_path) != 0)
    return;

  if (receive)
    obex_incoming_accept_active(service);
  else
    obex_incoming_reject_active(service, OBEX_ERROR_REJECTED, "Bluetooth file transfer rejected");
}

void
obex_service_send_file_to_device(ObexService *service,
                                 const char *device_path,
                                 const char *file_path)
{
  obex_outgoing_send_file_to_device(service, device_path, file_path);
}

void
obex_service_handle_transfer_status(ObexService *service,
                                    const char *transfer_path,
                                    const char *status)
{
  if (obex_incoming_handle_transfer_status(service, transfer_path, status))
    return;

  obex_outgoing_handle_transfer_status(service, transfer_path, status);
}
