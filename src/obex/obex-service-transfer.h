#pragma once

#include "obex/obex-service.h"

void obex_service_respond_to_file_notification(ObexService *service,
                                               const char *transfer_path,
                                               gboolean receive);
void obex_service_send_file_to_device(ObexService *service,
                                      const char *device_path,
                                      const char *file_path);
void obex_service_handle_transfer_status(ObexService *service,
                                         const char *transfer_path,
                                         const char *status);
