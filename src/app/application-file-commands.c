#include "app/application-file-commands.h"

#include "app/application-obex-service-access.h"
#include "obex/obex-service-transfer.h"

void
application_commands_send_file_to_device(Application *self,
                                         const char *device_path,
                                         const char *file_path)
{
  ObexService *obex_service = application_services_obex_service(self);

  if (obex_service && device_path && file_path)
    obex_service_send_file_to_device(obex_service, device_path, file_path);
}
