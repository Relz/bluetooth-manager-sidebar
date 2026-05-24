#pragma once

#include "obex/obex-service.h"

void obex_outgoing_send_file_to_device(ObexService *service,
                                       const char *device_path,
                                       const char *file_path);
