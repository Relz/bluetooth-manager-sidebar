#pragma once

#include "obex/obex-service.h"

char *obex_service_dup_received_directory(ObexService *service);
char *obex_service_dup_device_address(ObexService *service, const char *device_path);
gboolean obex_service_auto_accept_trusted(ObexService *service, const char *address);
char *obex_service_dup_device_name(ObexService *service,
                                   const char *address,
                                   const char *fallback);
