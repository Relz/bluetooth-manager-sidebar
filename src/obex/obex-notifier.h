#pragma once

#include "obex/obex-service-callbacks.h"

void obex_notifier_notify(const ObexStatusSink *sink,
                               gpointer user_data,
                               gboolean disposed,
                               const char *message);
void obex_notifier_refresh(const ObexStatusSink *sink,
                                gpointer user_data,
                                gboolean disposed);
void obex_notifier_set_status(char **status_message,
                                   const ObexStatusSink *sink,
                                   gpointer user_data,
                                   gboolean disposed,
                                   const char *message);
void obex_notifier_send_notification(const ObexNotificationSink *sink,
                                          gpointer user_data,
                                          const char *id,
                                          GNotification *notification);
void obex_notifier_withdraw_notification(const ObexNotificationSink *sink,
                                              gpointer user_data,
                                              const char *id);
