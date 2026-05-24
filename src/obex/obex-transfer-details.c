#include "obex/obex-transfer-details.h"

#include "obex/obex-dbus.h"

static char *
obex_variant_dup_string(GVariant *dict, const char *key)
{
  g_autoptr(GVariant) value = dict ? g_variant_lookup_value(dict, key, NULL) : NULL;
  g_autoptr(GVariant) unboxed = NULL;
  GVariant *actual = NULL;

  if (!value)
    return NULL;
  actual = value;
  if (g_variant_is_of_type(value, G_VARIANT_TYPE_VARIANT)) {
    unboxed = g_variant_get_variant(value);
    actual = unboxed;
  }
  if (g_variant_is_of_type(actual, G_VARIANT_TYPE_STRING) ||
      g_variant_is_of_type(actual, G_VARIANT_TYPE_OBJECT_PATH))
    return g_strdup(g_variant_get_string(actual, NULL));

  return NULL;
}

static gboolean
obex_variant_get_uint64(GVariant *dict, const char *key, guint64 *out_value)
{
  g_autoptr(GVariant) value = dict ? g_variant_lookup_value(dict, key, NULL) : NULL;
  g_autoptr(GVariant) unboxed = NULL;
  GVariant *actual = NULL;

  if (!value)
    return FALSE;
  actual = value;
  if (g_variant_is_of_type(value, G_VARIANT_TYPE_VARIANT)) {
    unboxed = g_variant_get_variant(value);
    actual = unboxed;
  }

  if (g_variant_is_of_type(actual, G_VARIANT_TYPE_BYTE))
    *out_value = g_variant_get_byte(actual);
  else if (g_variant_is_of_type(actual, G_VARIANT_TYPE_UINT16))
    *out_value = g_variant_get_uint16(actual);
  else if (g_variant_is_of_type(actual, G_VARIANT_TYPE_UINT32))
    *out_value = g_variant_get_uint32(actual);
  else if (g_variant_is_of_type(actual, G_VARIANT_TYPE_UINT64))
    *out_value = g_variant_get_uint64(actual);
  else if (g_variant_is_of_type(actual, G_VARIANT_TYPE_INT16))
    *out_value = MAX(0, g_variant_get_int16(actual));
  else if (g_variant_is_of_type(actual, G_VARIANT_TYPE_INT32))
    *out_value = MAX(0, g_variant_get_int32(actual));
  else if (g_variant_is_of_type(actual, G_VARIANT_TYPE_INT64))
    *out_value = MAX(0, g_variant_get_int64(actual));
  else
    return FALSE;

  return TRUE;
}

static GVariant *
obex_get_all_properties(GDBusConnection *connection, const char *path, const char *interface)
{
  g_autoptr(GError) error = NULL;
  g_autoptr(GVariant) reply = NULL;
  GVariant *dict = NULL;

  if (!connection || !path || !interface)
    return NULL;

  reply = g_dbus_connection_call_sync(connection,
                                      OBEX_BUS_NAME,
                                      path,
                                      DBUS_PROPERTIES_INTERFACE,
                                      "GetAll",
                                      g_variant_new("(s)", interface),
                                      G_VARIANT_TYPE("(a{sv})"),
                                      G_DBUS_CALL_FLAGS_NONE,
                                      OBEX_GET_PROPERTIES_TIMEOUT_MS,
                                      NULL,
                                      &error);
  if (!reply)
    return NULL;

  g_variant_get(reply, "(@a{sv})", &dict);
  return dict;
}

ObexTransferDetails *
obex_transfer_details_read(GDBusConnection *connection,
                           const char *transfer_path)
{
  ObexTransferDetails *details = g_new0(ObexTransferDetails, 1);
  g_autoptr(GVariant) transfer_props = NULL;
  g_autoptr(GVariant) session_props = NULL;

  details->path = g_strdup(transfer_path);
  transfer_props = obex_get_all_properties(connection, transfer_path, OBEX_TRANSFER_INTERFACE);
  details->filename = obex_variant_dup_string(transfer_props, "Filename");
  details->name = obex_variant_dup_string(transfer_props, "Name");
  details->session_path = obex_variant_dup_string(transfer_props, "Session");
  details->has_size = obex_variant_get_uint64(transfer_props, "Size", &details->size);

  if (details->session_path) {
    session_props = obex_get_all_properties(connection, details->session_path, OBEX_SESSION_INTERFACE);
    details->destination = obex_variant_dup_string(session_props, "Destination");
  }

  return details;
}

void
obex_transfer_details_free(ObexTransferDetails *details)
{
  if (!details)
    return;

  g_free(details->path);
  g_free(details->filename);
  g_free(details->name);
  g_free(details->session_path);
  g_free(details->destination);
  g_free(details);
}

char *
obex_transfer_status_from_properties(GVariant *properties)
{
  return obex_variant_dup_string(properties, "Status");
}
