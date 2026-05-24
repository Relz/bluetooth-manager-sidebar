#pragma once

#include <gio/gio.h>

typedef void (*ObexCreateSessionFinished)(GDBusConnection *connection,
                                            const char *session_path,
                                            const GError *error,
                                            gpointer user_data);
typedef void (*ObexSendFileFinished)(GDBusConnection *connection,
                                       const char *transfer_path,
                                       GVariant *properties,
                                       const GError *error,
                                       gpointer user_data);

gboolean obex_outgoing_dbus_create_session(GDBusConnection *connection,
                                           const char *address,
                                           ObexCreateSessionFinished finished,
                                           gpointer user_data);
gboolean obex_outgoing_dbus_send_file(GDBusConnection *connection,
                                      const char *session_path,
                                      const char *file_path,
                                      ObexSendFileFinished finished,
                                      gpointer user_data);
