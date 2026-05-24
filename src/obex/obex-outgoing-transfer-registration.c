#include "obex/obex-outgoing-transfer-registration.h"

#include "obex/obex-outgoing-status.h"
#include "obex/obex-sender.h"
#include "obex/obex-service-policy-private.h"
#include "obex/obex-service-registry-private.h"
#include "obex/obex-transfer-details.h"

gboolean
obex_outgoing_transfer_registration_store_if_pending(ObexOutgoingRequest *request,
                                                     const char *transfer_path,
                                                     GVariant *properties,
                                                     char **out_file_name)
{
  g_autofree char *status = obex_transfer_status_from_properties(properties);
  g_autofree char *file_name = g_path_get_basename(request->file_path);
  g_autofree char *device_name = obex_service_dup_device_name(request->service,
                                                              request->address,
                                                              "Bluetooth device");

  if (out_file_name)
    *out_file_name = NULL;

  if (obex_outgoing_settle_transfer_status(request->service,
                                           transfer_path,
                                           request->session_path,
                                           file_name,
                                           device_name,
                                           status))
    return FALSE;

  ObexOutgoingTransfer *transfer = obex_sender_transfer_new(request->session_path,
                                                               file_name,
                                                               device_name);

  obex_service_store_outgoing_transfer(request->service, transfer_path, transfer);
  if (out_file_name)
    *out_file_name = g_steal_pointer(&file_name);
  return TRUE;
}
