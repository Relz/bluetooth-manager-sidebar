#include "obex/obex-transfer-registry.h"

struct _ObexTransferRegistry {
  GHashTable *incoming;
  GHashTable *outgoing;
};

ObexTransferRegistry *
obex_transfer_registry_new(void)
{
  ObexTransferRegistry *registry = g_new0(ObexTransferRegistry, 1);

  registry->incoming = g_hash_table_new_full(g_str_hash,
                                             g_str_equal,
                                             g_free,
                                             (GDestroyNotify)obex_incoming_transfer_free);
  registry->outgoing = g_hash_table_new_full(g_str_hash,
                                             g_str_equal,
                                             g_free,
                                             (GDestroyNotify)obex_outgoing_transfer_free);
  return registry;
}

void
obex_transfer_registry_free(ObexTransferRegistry *registry)
{
  if (!registry)
    return;

  g_clear_pointer(&registry->incoming, g_hash_table_destroy);
  g_clear_pointer(&registry->outgoing, g_hash_table_destroy);
  g_free(registry);
}

void
obex_transfer_registry_shutdown(ObexTransferRegistry *registry,
                                GHFunc incoming_cleanup,
                                GHFunc outgoing_cleanup,
                                gpointer user_data)
{
  if (!registry)
    return;

  if (registry->incoming) {
    if (incoming_cleanup)
      g_hash_table_foreach(registry->incoming, incoming_cleanup, user_data);
    g_hash_table_remove_all(registry->incoming);
  }
  if (registry->outgoing) {
    if (outgoing_cleanup)
      g_hash_table_foreach(registry->outgoing, outgoing_cleanup, user_data);
    g_hash_table_remove_all(registry->outgoing);
  }
}

ObexIncomingTransfer *
obex_transfer_registry_lookup_incoming(ObexTransferRegistry *registry,
                                       const char *transfer_path)
{
  return (registry && transfer_path) ? g_hash_table_lookup(registry->incoming, transfer_path) : NULL;
}

void
obex_transfer_registry_store_incoming(ObexTransferRegistry *registry,
                                      const char *transfer_path,
                                      ObexIncomingTransfer *transfer)
{
  if (registry && transfer_path && transfer)
    g_hash_table_replace(registry->incoming, g_strdup(transfer_path), transfer);
}

void
obex_transfer_registry_remove_incoming(ObexTransferRegistry *registry,
                                       const char *transfer_path)
{
  if (registry && transfer_path)
    g_hash_table_remove(registry->incoming, transfer_path);
}

ObexOutgoingTransfer *
obex_transfer_registry_lookup_outgoing(ObexTransferRegistry *registry,
                                       const char *transfer_path)
{
  return (registry && transfer_path) ? g_hash_table_lookup(registry->outgoing, transfer_path) : NULL;
}

void
obex_transfer_registry_store_outgoing(ObexTransferRegistry *registry,
                                      const char *transfer_path,
                                      ObexOutgoingTransfer *transfer)
{
  if (registry && transfer_path && transfer)
    g_hash_table_replace(registry->outgoing, g_strdup(transfer_path), transfer);
}

void
obex_transfer_registry_remove_outgoing(ObexTransferRegistry *registry,
                                       const char *transfer_path)
{
  if (registry && transfer_path)
    g_hash_table_remove(registry->outgoing, transfer_path);
}
