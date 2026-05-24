#include "obex/obex-service-monitor-private.h"

#include "obex/obex-service-components.h"
#include "obex/obex-service-private.h"

void
obex_service_take_transfer_monitor(ObexService *service,
                                   ObexTransferMonitor *monitor)
{
  obex_service_components_take_transfer_monitor(obex_service_components(service), monitor);
}

void
obex_service_clear_transfer_monitor(ObexService *service)
{
  obex_service_components_clear_transfer_monitor(obex_service_components(service));
}
