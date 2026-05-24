#include "obex/obex-outgoing-request.h"

#include "obex/obex-sender.h"
#include "obex/obex-service-agent-private.h"
#include "obex/obex-service-policy-private.h"
#include "obex/obex-service-private.h"

ObexOutgoingRequestStatus
obex_outgoing_request_prepare(ObexService *service,
                              const char *device_path,
                              const char *file_path,
                              ObexOutgoingRequest **out_request,
                              char **out_detail)
{
  ObexOutgoingRequest *request = NULL;

  g_return_val_if_fail(out_request != NULL, OBEX_OUTGOING_REQUEST_SERVICE_DISPOSED);

  *out_request = NULL;
  if (out_detail)
    *out_detail = NULL;

  if (obex_service_is_disposed(service))
    return OBEX_OUTGOING_REQUEST_SERVICE_DISPOSED;

  if (!obex_service_connection(service))
    return OBEX_OUTGOING_REQUEST_BUS_UNAVAILABLE;

  if (!obex_sender_validate_file(file_path, out_detail))
    return OBEX_OUTGOING_REQUEST_FILE_INVALID;

  request = g_new0(ObexOutgoingRequest, 1);
  request->service = obex_service_ref(service);
  request->device_path = g_strdup(device_path);
  request->file_path = g_strdup(file_path);
  request->address = obex_service_dup_device_address(service, device_path);
  if (!request->address || !*request->address) {
    obex_outgoing_request_free(request);
    return OBEX_OUTGOING_REQUEST_ADDRESS_UNAVAILABLE;
  }

  *out_request = request;
  return OBEX_OUTGOING_REQUEST_READY;
}
