#include "obex/obex-service-registry-private.h"

#include "obex/obex-incoming-controller.h"
#include "obex/obex-outgoing-status.h"
#include "obex/obex-service-components.h"
#include "obex/obex-service-private.h"

ObexTransferRegistry *
obex_service_transfer_registry(ObexService *service)
{
  return obex_service_components_transfers(obex_service_components(service));
}

void
obex_service_transfer_registry_shutdown(ObexService *service)
{
  obex_transfer_registry_shutdown(obex_service_transfer_registry(service),
                                  obex_incoming_cleanup_transfer_foreach,
                                  obex_outgoing_remove_session_foreach,
                                  service);
}

ObexIncomingTransfer *
obex_service_lookup_incoming_transfer(ObexService *service, const char *transfer_path)
{
  return obex_transfer_registry_lookup_incoming(obex_service_transfer_registry(service),
                                               transfer_path);
}

void
obex_service_store_incoming_transfer(ObexService *service,
                                     const char *transfer_path,
                                     ObexIncomingTransfer *transfer)
{
  obex_transfer_registry_store_incoming(obex_service_transfer_registry(service),
                                        transfer_path,
                                        transfer);
}

void
obex_service_remove_incoming_transfer(ObexService *service, const char *transfer_path)
{
  obex_transfer_registry_remove_incoming(obex_service_transfer_registry(service), transfer_path);
}

ObexOutgoingTransfer *
obex_service_lookup_outgoing_transfer(ObexService *service, const char *transfer_path)
{
  return obex_transfer_registry_lookup_outgoing(obex_service_transfer_registry(service),
                                               transfer_path);
}

void
obex_service_store_outgoing_transfer(ObexService *service,
                                     const char *transfer_path,
                                     ObexOutgoingTransfer *transfer)
{
  obex_transfer_registry_store_outgoing(obex_service_transfer_registry(service),
                                        transfer_path,
                                        transfer);
}

void
obex_service_remove_outgoing_transfer(ObexService *service, const char *transfer_path)
{
  obex_transfer_registry_remove_outgoing(obex_service_transfer_registry(service), transfer_path);
}
