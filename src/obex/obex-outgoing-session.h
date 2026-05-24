#pragma once

#include <gio/gio.h>

#include "obex/obex-service.h"

typedef struct _ObexOutgoingRequest ObexOutgoingRequest;

typedef void (*ObexOutgoingSessionEvent)(ObexOutgoingRequest *request,
                                            gpointer user_data);
typedef void (*ObexOutgoingSessionFailed)(ObexOutgoingRequest *request,
                                             const GError *error,
                                             gpointer user_data);
typedef void (*ObexOutgoingSessionStarted)(ObexOutgoingRequest *request,
                                              const char *transfer_path,
                                              GVariant *properties,
                                              gpointer user_data);

typedef struct {
  gpointer user_data;
  ObexOutgoingSessionEvent ready_to_send;
  ObexOutgoingSessionEvent prepare_bus_unavailable;
  ObexOutgoingSessionFailed prepare_failed;
  ObexOutgoingSessionEvent send_bus_unavailable;
  ObexOutgoingSessionFailed send_failed;
  ObexOutgoingSessionStarted send_started;
} ObexOutgoingSessionCallbacks;

struct _ObexOutgoingRequest {
  ObexService *service;
  char *device_path;
  char *file_path;
  char *address;
  char *session_path;
  ObexOutgoingSessionCallbacks callbacks;
};

void obex_outgoing_request_free(ObexOutgoingRequest *request);
void obex_outgoing_session_start(ObexService *service,
                                 ObexOutgoingRequest *request,
                                 const ObexOutgoingSessionCallbacks *callbacks);
