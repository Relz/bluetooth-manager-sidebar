#include "obex/obex-service-notification-private.h"

#include "obex/obex-dbus.h"
#include "obex/obex-service-components.h"
#include "obex/obex-service-context.h"
#include "obex/obex-service-private.h"

#include "obex/obex-notifications.h"
#include "obex/obex-notifier.h"

static ObexServiceContext *
obex_service_notification_context(ObexService *service)
{
  return obex_service_components_context(obex_service_components(service));
}

static const ObexStatusSink *
obex_service_status_sink(ObexService *service)
{
  return obex_service_context_status_sink(obex_service_notification_context(service));
}

static const ObexNotificationSink *
obex_service_notification_sink(ObexService *service)
{
  return obex_service_context_notification_sink(obex_service_notification_context(service));
}

static gpointer
obex_service_callback_user_data(ObexService *service)
{
  return obex_service_context_user_data(obex_service_notification_context(service));
}

static char **
obex_service_status_message_slot(ObexService *service)
{
  return obex_service_context_status_message_slot(obex_service_notification_context(service));
}

void
obex_service_notify(ObexService *service, const char *message)
{
  if (service)
    obex_notifier_notify(obex_service_status_sink(service),
                          obex_service_callback_user_data(service),
                          obex_service_is_disposed(service),
                          message);
}

void
obex_service_send_notification(ObexService *service,
                               const char *id,
                               GNotification *notification)
{
  if (service)
    obex_notifier_send_notification(obex_service_notification_sink(service),
                                     obex_service_callback_user_data(service),
                                     id,
                                     notification);
}

void
obex_service_withdraw_notification(ObexService *service, const char *id)
{
  if (service)
    obex_notifier_withdraw_notification(obex_service_notification_sink(service),
                                         obex_service_callback_user_data(service),
                                         id);
}

void
obex_service_set_status(ObexService *service, const char *message)
{
  if (!service)
    return;

  obex_notifier_set_status(obex_service_status_message_slot(service),
                            obex_service_status_sink(service),
                            obex_service_callback_user_data(service),
                            obex_service_is_disposed(service),
                            message);
}

void
obex_service_send_completed_notification(ObexService *service, const char *final_path)
{
  g_autoptr(GNotification) notification = NULL;

  notification = obex_notification_new_completed(final_path);
  obex_service_send_notification(service, OBEX_COMPLETED_NOTIFICATION_ID, notification);
}
