#include "obex/obex-outgoing-session.h"

#include "obex/obex-outgoing-dbus.h"
#include "obex/obex-service-agent-private.h"
#include "obex/obex-service-private.h"
#include "obex/obex-service-session-private.h"

static void obex_send_file_ready(GDBusConnection *connection,
                                 const char *transfer_path,
                                 GVariant *properties,
                                 const GError *error,
                                 gpointer user_data);

static void
obex_outgoing_request_remove_session(ObexOutgoingRequest *request,
                                     GDBusConnection *connection)
{
  if (!request || !request->session_path)
    return;

  if (obex_service_is_disposed(request->service))
    obex_service_remove_session_on_connection(connection, request->session_path);
  else
    obex_service_remove_session(request->service, request->session_path);
}

static void
obex_outgoing_session_emit_event(ObexOutgoingRequest *request,
                                 ObexOutgoingSessionEvent event)
{
  if (event)
    event(request, request->callbacks.user_data);
}

static void
obex_outgoing_session_emit_failed(ObexOutgoingRequest *request,
                                  ObexOutgoingSessionFailed failed,
                                  const GError *error)
{
  if (failed)
    failed(request, error, request->callbacks.user_data);
}

static void
obex_outgoing_session_emit_started(ObexOutgoingRequest *request,
                                   const char *transfer_path,
                                   GVariant *properties)
{
  if (request->callbacks.send_started)
    request->callbacks.send_started(request,
                                    transfer_path,
                                    properties,
                                    request->callbacks.user_data);
}

static void
obex_outgoing_request_handle_create_failure(ObexOutgoingRequest *request,
                                             const GError *error)
{
  obex_outgoing_session_emit_failed(request, request->callbacks.prepare_failed, error);
  obex_outgoing_request_free(request);
}

static void
obex_outgoing_request_handle_send_failure(ObexOutgoingRequest *request,
                                          GDBusConnection *connection,
                                          const GError *error)
{
  obex_outgoing_session_emit_failed(request, request->callbacks.send_failed, error);
  obex_outgoing_request_remove_session(request, connection);
  obex_outgoing_request_free(request);
}

static void
obex_outgoing_request_begin_send(ObexOutgoingRequest *request)
{
  ObexService *service = request->service;

  obex_outgoing_session_emit_event(request, request->callbacks.ready_to_send);
  if (!obex_outgoing_dbus_send_file(obex_service_connection(service),
                                    request->session_path,
                                    request->file_path,
                                    obex_send_file_ready,
                                    request)) {
    obex_outgoing_session_emit_event(request, request->callbacks.send_bus_unavailable);
    obex_service_remove_session(service, request->session_path);
    obex_outgoing_request_free(request);
  }
}

static void
obex_outgoing_request_handle_send_started(ObexOutgoingRequest *request,
                                          const char *transfer_path,
                                          GVariant *properties)
{
  obex_outgoing_session_emit_started(request, transfer_path, properties);
  obex_outgoing_request_free(request);
}

void
obex_outgoing_request_free(ObexOutgoingRequest *request)
{
  if (!request)
    return;

  obex_service_unref(request->service);
  g_free(request->device_path);
  g_free(request->file_path);
  g_free(request->address);
  g_free(request->session_path);
  g_free(request);
}

static void
obex_create_session_ready(GDBusConnection *connection,
                          const char *session_path,
                          const GError *error,
                          gpointer user_data)
{
  ObexOutgoingRequest *request = user_data;
  ObexService *service = request->service;

  if (!session_path) {
    obex_outgoing_request_handle_create_failure(request, error);
    return;
  }

  request->session_path = g_strdup(session_path);
  if (obex_service_is_disposed(service)) {
    obex_outgoing_request_remove_session(request, connection);
    obex_outgoing_request_free(request);
    return;
  }

  obex_outgoing_request_begin_send(request);
}

static void
obex_send_file_ready(GDBusConnection *connection,
                     const char *transfer_path,
                     GVariant *properties,
                     const GError *error,
                     gpointer user_data)
{
  ObexOutgoingRequest *request = user_data;

  if (!transfer_path) {
    obex_outgoing_request_handle_send_failure(request, connection, error);
    return;
  }

  if (obex_service_is_disposed(request->service)) {
    obex_outgoing_request_remove_session(request, connection);
    obex_outgoing_request_free(request);
    return;
  }

  obex_outgoing_request_handle_send_started(request, transfer_path, properties);
}

void
obex_outgoing_session_start(ObexService *service,
                            ObexOutgoingRequest *request,
                            const ObexOutgoingSessionCallbacks *callbacks)
{
  request->callbacks = callbacks ? *callbacks : (ObexOutgoingSessionCallbacks){ 0 };
  if (!obex_outgoing_dbus_create_session(obex_service_connection(service),
                                          request->address,
                                          obex_create_session_ready,
                                          request)) {
    obex_outgoing_session_emit_event(request, request->callbacks.prepare_bus_unavailable);
    obex_outgoing_request_free(request);
  }
}
