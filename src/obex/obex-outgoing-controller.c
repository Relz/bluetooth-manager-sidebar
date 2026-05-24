#include "obex/obex-outgoing-controller.h"

#include "obex/obex-outgoing-request.h"
#include "obex/obex-outgoing-session.h"
#include "obex/obex-outgoing-session-events.h"

void
obex_outgoing_send_file_to_device(ObexService *service,
                                  const char *device_path,
                                  const char *file_path)
{
  ObexOutgoingRequest *request = NULL;
  ObexOutgoingRequestStatus status;
  g_autofree char *detail = NULL;

  status = obex_outgoing_request_prepare(service,
                                         device_path,
                                         file_path,
                                         &request,
                                         &detail);
  if (status != OBEX_OUTGOING_REQUEST_READY) {
    obex_outgoing_session_events_report_request_status(service, status, detail);
    return;
  }

  ObexOutgoingSessionCallbacks callbacks = obex_outgoing_session_events_callbacks();

  obex_outgoing_session_events_report_preparing(service);
  obex_outgoing_session_start(service, request, &callbacks);
}
