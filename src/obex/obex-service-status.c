#include "obex/obex-service-status.h"

#include "obex/obex-service-components.h"
#include "obex/obex-service-context.h"
#include "obex/obex-service-private.h"

const char *
obex_service_status_message(ObexService *service)
{
  return obex_service_context_status_message_value(
    obex_service_components_context(obex_service_components(service)));
}
