#include "obex/obex-outgoing-dbus.h"

#include "obex/obex-dbus.h"
#include "obex/obex-sender.h"

typedef struct {
  ObexCreateSessionFinished finished;
  gpointer user_data;
} ObexCreateSessionCallContext;

typedef struct {
  ObexSendFileFinished finished;
  gpointer user_data;
} ObexSendFileCallContext;

static ObexCreateSessionCallContext *
obex_create_session_call_context_new(ObexCreateSessionFinished finished,
                                     gpointer user_data)
{
  ObexCreateSessionCallContext *context = g_new0(ObexCreateSessionCallContext, 1);

  context->finished = finished;
  context->user_data = user_data;
  return context;
}

static ObexSendFileCallContext *
obex_send_file_call_context_new(ObexSendFileFinished finished,
                                gpointer user_data)
{
  ObexSendFileCallContext *context = g_new0(ObexSendFileCallContext, 1);

  context->finished = finished;
  context->user_data = user_data;
  return context;
}

static void
obex_create_session_ready(GObject *source_object, GAsyncResult *result, gpointer user_data)
{
  ObexCreateSessionCallContext *context = user_data;
  g_autoptr(GError) error = NULL;
  g_autoptr(GVariant) reply = NULL;
  g_autofree char *session_path = NULL;

  reply = g_dbus_connection_call_finish(G_DBUS_CONNECTION(source_object), result, &error);
  if (reply)
    g_variant_get(reply, "(o)", &session_path);

  if (context->finished)
    context->finished(G_DBUS_CONNECTION(source_object), session_path, error, context->user_data);
  g_free(context);
}

static void
obex_send_file_ready(GObject *source_object, GAsyncResult *result, gpointer user_data)
{
  ObexSendFileCallContext *context = user_data;
  g_autoptr(GError) error = NULL;
  g_autoptr(GVariant) reply = NULL;
  g_autoptr(GVariant) properties = NULL;
  g_autofree char *transfer_path = NULL;

  reply = g_dbus_connection_call_finish(G_DBUS_CONNECTION(source_object), result, &error);
  if (reply)
    g_variant_get(reply, "(o@a{sv})", &transfer_path, &properties);

  if (context->finished)
    context->finished(G_DBUS_CONNECTION(source_object), transfer_path, properties, error, context->user_data);
  g_free(context);
}

gboolean
obex_outgoing_dbus_create_session(GDBusConnection *connection,
                                  const char *address,
                                  ObexCreateSessionFinished finished,
                                  gpointer user_data)
{
  ObexCreateSessionCallContext *context = NULL;
  GVariantBuilder builder;

  if (!connection || !address || !*address)
    return FALSE;

  context = obex_create_session_call_context_new(finished, user_data);
  g_variant_builder_init(&builder, G_VARIANT_TYPE("a{sv}"));
  obex_sender_add_session_options(&builder);
  g_dbus_connection_call(connection,
                         OBEX_BUS_NAME,
                         OBEX_ROOT_PATH,
                         OBEX_CLIENT_INTERFACE,
                         "CreateSession",
                         g_variant_new("(sa{sv})", address, &builder),
                         G_VARIANT_TYPE("(o)"),
                         G_DBUS_CALL_FLAGS_NONE,
                         OBEX_CREATE_SESSION_TIMEOUT_MS,
                         NULL,
                         obex_create_session_ready,
                         context);
  return TRUE;
}

gboolean
obex_outgoing_dbus_send_file(GDBusConnection *connection,
                             const char *session_path,
                             const char *file_path,
                             ObexSendFileFinished finished,
                             gpointer user_data)
{
  ObexSendFileCallContext *context = NULL;

  if (!connection || !session_path || !file_path)
    return FALSE;

  context = obex_send_file_call_context_new(finished, user_data);
  g_dbus_connection_call(connection,
                         OBEX_BUS_NAME,
                         session_path,
                         OBEX_OBJECT_PUSH_INTERFACE,
                         "SendFile",
                         g_variant_new("(s)", file_path),
                         G_VARIANT_TYPE("(oa{sv})"),
                         G_DBUS_CALL_FLAGS_NONE,
                         OBEX_SEND_FILE_TIMEOUT_MS,
                         NULL,
                         obex_send_file_ready,
                         context);
  return TRUE;
}
