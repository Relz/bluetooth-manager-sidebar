#include "obex/obex-sender.h"

struct _ObexOutgoingTransfer {
  char *session_path;
  char *file_name;
  char *device_name;
};

ObexOutgoingTransfer *
obex_sender_transfer_new(const char *session_path, const char *file_name, const char *device_name)
{
  ObexOutgoingTransfer *transfer = g_new0(ObexOutgoingTransfer, 1);

  transfer->session_path = g_strdup(session_path);
  transfer->file_name = g_strdup(file_name);
  transfer->device_name = g_strdup(device_name);
  return transfer;
}

void
obex_outgoing_transfer_free(ObexOutgoingTransfer *transfer)
{
  if (!transfer)
    return;

  g_free(transfer->session_path);
  g_free(transfer->file_name);
  g_free(transfer->device_name);
  g_free(transfer);
}

const char *
obex_outgoing_transfer_session_path(ObexOutgoingTransfer *transfer)
{
  return transfer ? transfer->session_path : NULL;
}

const char *
obex_outgoing_transfer_file_name(ObexOutgoingTransfer *transfer)
{
  return transfer ? transfer->file_name : NULL;
}

const char *
obex_outgoing_transfer_device_name(ObexOutgoingTransfer *transfer)
{
  return transfer ? transfer->device_name : NULL;
}

gboolean
obex_sender_validate_file(const char *file_path, char **out_message)
{
  if (g_file_test(file_path, G_FILE_TEST_IS_REGULAR))
    return TRUE;

  if (out_message)
    *out_message = g_strdup("Unable to send file: selected path is not a file");
  return FALSE;
}

void
obex_sender_add_session_options(GVariantBuilder *builder)
{
  g_variant_builder_add(builder, "{sv}", "Target", g_variant_new_string("opp"));
}
