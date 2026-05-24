#pragma once

#include "obex/obex-outgoing-session.h"

typedef enum {
  OBEX_OUTGOING_REQUEST_READY,
  OBEX_OUTGOING_REQUEST_SERVICE_DISPOSED,
  OBEX_OUTGOING_REQUEST_BUS_UNAVAILABLE,
  OBEX_OUTGOING_REQUEST_FILE_INVALID,
  OBEX_OUTGOING_REQUEST_ADDRESS_UNAVAILABLE,
} ObexOutgoingRequestStatus;

ObexOutgoingRequestStatus obex_outgoing_request_prepare(ObexService *service,
                                                           const char *device_path,
                                                           const char *file_path,
                                                           ObexOutgoingRequest **out_request,
                                                           char **out_detail);
