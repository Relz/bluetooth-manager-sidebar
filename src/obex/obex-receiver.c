#include "obex/obex-receiver.h"

#include "obex/obex-transfer-storage.h"

struct _ObexIncomingTransfer {
  char *staging_path;
  char *final_path;
};

ObexIncomingTransfer *
obex_receiver_prepare_transfer(const char *received_directory_config,
                                   const char *name,
                                   const char *filename,
                                   GError **error)
{
  ObexIncomingTransfer *transfer = g_new0(ObexIncomingTransfer, 1);
  g_autofree char *safe_name = obex_storage_safe_file_name(name && *name ? name : filename);
  g_autofree char *received_dir = obex_storage_ensure_received_directory(received_directory_config, error);
  g_autofree char *final_base = NULL;
  g_autofree char *staging_dir = NULL;
  g_autofree char *staging_base = NULL;

  if (!received_dir)
    goto fail;

  final_base = g_build_filename(received_dir, safe_name, NULL);
  transfer->final_path = obex_storage_unique_destination(final_base, error);
  if (!transfer->final_path)
    goto fail;

  staging_dir = obex_storage_staging_base_directory(filename);
  if (!obex_storage_ensure_directory(staging_dir, error))
    goto fail;

  staging_base = g_build_filename(staging_dir, safe_name, NULL);
  transfer->staging_path = obex_storage_unique_destination(staging_base, error);
  if (!transfer->staging_path)
    goto fail;

  return transfer;

fail:
  obex_incoming_transfer_free(transfer);
  return NULL;
}

void
obex_incoming_transfer_free(ObexIncomingTransfer *transfer)
{
  if (!transfer)
    return;

  g_free(transfer->staging_path);
  g_free(transfer->final_path);
  g_free(transfer);
}

const char *
obex_incoming_transfer_staging_path(ObexIncomingTransfer *transfer)
{
  return transfer ? transfer->staging_path : NULL;
}

const char *
obex_incoming_transfer_final_path(ObexIncomingTransfer *transfer)
{
  return transfer ? transfer->final_path : NULL;
}

gboolean
obex_receiver_finalize_transfer(ObexIncomingTransfer *transfer,
                                    char **out_final_path,
                                    GError **error)
{
  if (!transfer)
    return FALSE;

  return obex_storage_finalize_incoming_transfer(transfer->staging_path,
                                                     transfer->final_path,
                                                     out_final_path,
                                                     error);
}

void
obex_receiver_cleanup_transfer(ObexIncomingTransfer *transfer)
{
  if (transfer)
    obex_storage_cleanup_incoming_transfer(transfer->staging_path);
}
