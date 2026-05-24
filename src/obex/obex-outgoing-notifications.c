#include "obex/obex-outgoing-notifications.h"

#include "core/display.h"
#include "obex/obex-dbus.h"
#include "obex/obex-notifications.h"
#include "obex/obex-service-notification-private.h"

void
obex_outgoing_notifications_notify_preparing(ObexService *service)
{
  obex_service_notify(service, "Preparing file transfer...");
}

void
obex_outgoing_notifications_notify_prepare_failed(ObexService *service, const char *detail)
{
  g_autofree char *safe_detail = display_dup_safe_text(detail, "unknown error");
  g_autofree char *message = g_strdup_printf("Unable to prepare file transfer: %s",
                                             safe_detail ? safe_detail : "unknown error");

  obex_service_notify(service, message);
}

void
obex_outgoing_notifications_notify_prepare_bus_unavailable(ObexService *service)
{
  obex_service_notify(service, "Unable to prepare file transfer: OBEX session bus unavailable");
}

void
obex_outgoing_notifications_notify_sending(ObexService *service)
{
  obex_service_notify(service, "Sending file...");
}

void
obex_outgoing_notifications_notify_send_failed(ObexService *service, const char *detail)
{
  g_autofree char *safe_detail = display_dup_safe_text(detail, "unknown error");
  g_autofree char *message = g_strdup_printf("Unable to send file: %s",
                                             safe_detail ? safe_detail : "unknown error");

  obex_service_notify(service, message);
}

void
obex_outgoing_notifications_notify_send_bus_unavailable(ObexService *service)
{
  obex_service_notify(service, "Unable to send file: OBEX session bus unavailable");
}

void
obex_outgoing_notifications_notify_sending_file(ObexService *service, const char *file_name)
{
  g_autofree char *safe_file_name = display_dup_safe_text(file_name, "file");
  g_autofree char *message = g_strdup_printf("Sending %s", safe_file_name ? safe_file_name : "file");

  obex_service_notify(service, message);
}

void
obex_outgoing_notifications_notify_sent(ObexService *service, const char *file_name)
{
  g_autofree char *safe_file_name = display_dup_safe_text(file_name, "file");
  g_autofree char *message = g_strdup_printf("Sent %s", safe_file_name ? safe_file_name : "file");

  obex_service_notify(service, message);
}

void
obex_outgoing_notifications_notify_failed(ObexService *service)
{
  obex_service_notify(service, "Bluetooth file transfer failed");
}

void
obex_outgoing_notifications_notify_rejected(ObexService *service,
                                            const char *device_name,
                                            const char *file_name)
{
  g_autoptr(GNotification) notification = NULL;

  notification = obex_notification_new_outgoing_rejected(device_name, file_name);
  obex_service_send_notification(service, OBEX_OUTGOING_FAILED_NOTIFICATION_ID, notification);
}

void
obex_outgoing_notifications_notify_device_address_unavailable(ObexService *service)
{
  obex_service_notify(service, "Unable to send file: device address unavailable");
}

void
obex_outgoing_notifications_notify_file_validation_error(ObexService *service, const char *message)
{
  g_autofree char *safe_message = display_dup_safe_text(message, "Unable to send file");

  obex_service_notify(service, safe_message ? safe_message : "Unable to send file");
}
