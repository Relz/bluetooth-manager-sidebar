#pragma once

#include "obex/obex-outgoing-request.h"

ObexOutgoingSessionCallbacks obex_outgoing_session_events_callbacks(void);
void obex_outgoing_session_events_report_preparing(ObexService *service);
void obex_outgoing_session_events_report_request_status(ObexService *service,
                                                        ObexOutgoingRequestStatus status,
                                                        const char *detail);
