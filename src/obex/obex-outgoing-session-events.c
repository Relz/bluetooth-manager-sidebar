#include "obex/obex-outgoing-session-events.h"

#include <string.h>

#include "obex/obex-dbus.h"
#include "obex/obex-outgoing-notifications.h"
#include "obex/obex-outgoing-transfer-registration.h"
#include "obex/obex-service-policy-private.h"
#include "obex/obex-service-private.h"

static gboolean
obex_outgoing_session_events_error_is_rejection(const GError *error)
{
  g_autofree char *remote_error = NULL;
  g_autofree char *normalized = NULL;

  if (!error)
    return FALSE;

  remote_error = g_dbus_error_get_remote_error(error);
  if (g_strcmp0(remote_error, OBEX_ERROR_REJECTED) == 0)
    return TRUE;

  normalized = g_utf8_casefold(error->message ? error->message : "", -1);
  return normalized &&
         (strstr(normalized, "reject") ||
          strstr(normalized, "refus") ||
          strstr(normalized, "not accept") ||
          strstr(normalized, "declin") ||
          strstr(normalized, "denied"));
}

static void
obex_outgoing_session_events_notify_rejection(ObexOutgoingRequest *request)
{
  g_autofree char *device_name = NULL;
  g_autofree char *file_name = NULL;

  device_name = obex_service_dup_device_name(request->service,
                                             request->address,
                                             "Bluetooth device");
  file_name = g_path_get_basename(request->file_path);
  obex_outgoing_notifications_notify_rejected(request->service, device_name, file_name);
}

static void
obex_outgoing_session_events_on_ready_to_send(ObexOutgoingRequest *request,
                                             gpointer user_data)
{
  (void)user_data;
  if (!obex_service_is_disposed(request->service))
    obex_outgoing_notifications_notify_sending(request->service);
}

static void
obex_outgoing_session_events_on_prepare_bus_unavailable(ObexOutgoingRequest *request,
                                                       gpointer user_data)
{
  (void)user_data;
  if (!obex_service_is_disposed(request->service))
    obex_outgoing_notifications_notify_prepare_bus_unavailable(request->service);
}

static void
obex_outgoing_session_events_on_prepare_failed(ObexOutgoingRequest *request,
                                               const GError *error,
                                               gpointer user_data)
{
  (void)user_data;
  if (!obex_service_is_disposed(request->service))
    obex_outgoing_notifications_notify_prepare_failed(request->service,
                                                      error ? error->message : NULL);
}

static void
obex_outgoing_session_events_on_send_bus_unavailable(ObexOutgoingRequest *request,
                                                    gpointer user_data)
{
  (void)user_data;
  if (!obex_service_is_disposed(request->service))
    obex_outgoing_notifications_notify_send_bus_unavailable(request->service);
}

static void
obex_outgoing_session_events_on_send_failed(ObexOutgoingRequest *request,
                                            const GError *error,
                                            gpointer user_data)
{
  (void)user_data;
  if (!obex_service_is_disposed(request->service)) {
    obex_outgoing_notifications_notify_send_failed(request->service,
                                                    error ? error->message : NULL);
    if (obex_outgoing_session_events_error_is_rejection(error))
      obex_outgoing_session_events_notify_rejection(request);
  }
}

static void
obex_outgoing_session_events_on_send_started(ObexOutgoingRequest *request,
                                             const char *transfer_path,
                                             GVariant *properties,
                                             gpointer user_data)
{
  g_autofree char *file_name = NULL;

  (void)user_data;
  if (obex_service_is_disposed(request->service))
    return;

  if (obex_outgoing_transfer_registration_store_if_pending(request,
                                                           transfer_path,
                                                           properties,
                                                           &file_name))
    obex_outgoing_notifications_notify_sending_file(request->service, file_name);
}

ObexOutgoingSessionCallbacks
obex_outgoing_session_events_callbacks(void)
{
  return (ObexOutgoingSessionCallbacks){
    .ready_to_send = obex_outgoing_session_events_on_ready_to_send,
    .prepare_bus_unavailable = obex_outgoing_session_events_on_prepare_bus_unavailable,
    .prepare_failed = obex_outgoing_session_events_on_prepare_failed,
    .send_bus_unavailable = obex_outgoing_session_events_on_send_bus_unavailable,
    .send_failed = obex_outgoing_session_events_on_send_failed,
    .send_started = obex_outgoing_session_events_on_send_started,
  };
}

void
obex_outgoing_session_events_report_preparing(ObexService *service)
{
  obex_outgoing_notifications_notify_preparing(service);
}

void
obex_outgoing_session_events_report_request_status(ObexService *service,
                                                   ObexOutgoingRequestStatus status,
                                                   const char *detail)
{
  switch (status) {
    case OBEX_OUTGOING_REQUEST_SERVICE_DISPOSED:
    case OBEX_OUTGOING_REQUEST_READY:
      break;
    case OBEX_OUTGOING_REQUEST_BUS_UNAVAILABLE:
      obex_outgoing_notifications_notify_send_bus_unavailable(service);
      break;
    case OBEX_OUTGOING_REQUEST_FILE_INVALID:
      obex_outgoing_notifications_notify_file_validation_error(service, detail);
      break;
    case OBEX_OUTGOING_REQUEST_ADDRESS_UNAVAILABLE:
      obex_outgoing_notifications_notify_device_address_unavailable(service);
      break;
  }
}
