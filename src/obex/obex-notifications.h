#pragma once

#include <gio/gio.h>
#include <glib.h>

GNotification *obex_notification_new_authorization(const char *device_name,
                                                       const char *file_name,
                                                       gboolean has_size,
                                                       guint64 size,
                                                       const char *received_dir,
                                                       const char *transfer_path);
char *obex_notification_dup_authorization_toast(const char *device_name);
GNotification *obex_notification_new_completed(const char *final_path);
GNotification *obex_notification_new_outgoing_rejected(const char *device_name,
                                                       const char *file_name);
