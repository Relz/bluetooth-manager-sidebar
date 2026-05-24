#include "obex/obex-notifier.h"

void
obex_notifier_notify(const ObexStatusSink *sink,
                          gpointer user_data,
                          gboolean disposed,
                          const char *message)
{
  if (!disposed && sink && sink->notify && message && *message)
    sink->notify(message, user_data);
}

void
obex_notifier_refresh(const ObexStatusSink *sink,
                           gpointer user_data,
                           gboolean disposed)
{
  if (!disposed && sink && sink->refresh)
    sink->refresh(user_data);
}

void
obex_notifier_set_status(char **status_message,
                             const ObexStatusSink *sink,
                             gpointer user_data,
                             gboolean disposed,
                             const char *message)
{
  if (!status_message)
    return;

  g_free(*status_message);
  *status_message = g_strdup(message);
  obex_notifier_notify(sink, user_data, disposed, message);
  obex_notifier_refresh(sink, user_data, disposed);
}

void
obex_notifier_send_notification(const ObexNotificationSink *sink,
                                     gpointer user_data,
                                     const char *id,
                                     GNotification *notification)
{
  if (sink && sink->send_notification)
    sink->send_notification(id, notification, user_data);
}

void
obex_notifier_withdraw_notification(const ObexNotificationSink *sink,
                                         gpointer user_data,
                                         const char *id)
{
  if (sink && sink->withdraw_notification)
    sink->withdraw_notification(id, user_data);
}
