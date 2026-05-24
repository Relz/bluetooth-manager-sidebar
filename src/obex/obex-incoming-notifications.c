#include "obex/obex-incoming-notifications.h"

#include "core/display.h"
#include "obex/obex-dbus.h"
#include "obex/obex-notifications.h"
#include "obex/obex-receiver.h"
#include "obex/obex-received-directory.h"
#include "obex/obex-service-notification-private.h"
#include "obex/obex-service-policy-private.h"
#include "obex/obex-transfer-paths.h"

void
obex_incoming_notifications_withdraw(ObexService *service)
{
  obex_service_withdraw_notification(service, OBEX_INCOMING_NOTIFICATION_ID);
}

void
obex_incoming_notifications_send_authorization(ObexService *service,
                                               ObexTransferDetails *details)
{
  g_autofree char *received_dir_config = obex_service_dup_received_directory(service);
  g_autofree char *received_dir = obex_received_directory_expand(received_dir_config);
  g_autofree char *file_name = obex_storage_safe_file_name(details->name && *details->name ? details->name : details->filename);
  const char *fallback = details->destination && *details->destination ? details->destination : "Bluetooth device";
  g_autofree char *device_name = obex_service_dup_device_name(service, details->destination, fallback);
  g_autoptr(GNotification) notification = NULL;

  notification = obex_notification_new_authorization(device_name,
                                                      file_name,
                                                      details->has_size,
                                                      details->size,
                                                      received_dir,
                                                      details->path);
  obex_service_send_notification(service, OBEX_INCOMING_NOTIFICATION_ID, notification);

  g_autofree char *toast = obex_notification_dup_authorization_toast(device_name);
  obex_service_notify(service, toast);
}

char *
obex_incoming_notifications_notify_location_error(ObexService *service,
                                                  const char *detail)
{
  g_autofree char *safe_detail = display_dup_safe_text(detail, "unknown error");
  char *message = g_strdup_printf("Unable to use received files location: %s",
                                  safe_detail ? safe_detail : "unknown error");

  obex_service_notify(service, message);
  return message;
}

void
obex_incoming_notifications_notify_receiving(ObexService *service,
                                             ObexIncomingTransfer *transfer)
{
  g_autofree char *directory = g_path_get_dirname(obex_incoming_transfer_final_path(transfer));
  g_autofree char *safe_directory = display_dup_safe_text(directory, "destination folder");
  g_autofree char *message = g_strdup_printf("Receiving file in %s",
                                             safe_directory ? safe_directory : "destination folder");

  obex_service_notify(service, message);
}

void
obex_incoming_notifications_notify_save_error(ObexService *service,
                                              const char *detail)
{
  g_autofree char *safe_detail = display_dup_safe_text(detail, "unknown error");
  g_autofree char *message = g_strdup_printf("Unable to save received file: %s",
                                             safe_detail ? safe_detail : "unknown error");

  obex_service_notify(service, message);
}

void
obex_incoming_notifications_notify_saved(ObexService *service,
                                         const char *final_path)
{
  g_autofree char *safe_path = display_dup_safe_text(final_path, "destination file");
  g_autofree char *message = g_strdup_printf("Received file saved to %s",
                                             safe_path ? safe_path : "destination file");

  obex_service_notify(service, message);
  obex_service_send_completed_notification(service, final_path);
}

void
obex_incoming_notifications_notify_failed(ObexService *service)
{
  obex_service_notify(service, "Bluetooth file transfer failed");
}
