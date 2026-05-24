#include "app/application-adapter-info-model.h"

#include "app/application-page-status.h"
#include "bluez/bluez-client-query.h"

ApplicationAdapterInfoModel
application_adapter_info_model_new(Application *self, BluezClient *bluez)
{
  const char *adapter_path = bluez ? bluez_client_primary_adapter(bluez) : NULL;
  BluezProperties *adapter = application_page_adapter_properties(self, adapter_path);

  return (ApplicationAdapterInfoModel){
    .adapter_path = adapter_path,
    .adapter = adapter,
    .status_message = (!adapter_path || !adapter) ? application_page_dup_status_message(self) : NULL,
  };
}

void
application_adapter_info_model_clear(ApplicationAdapterInfoModel *model)
{
  if (!model)
    return;

  g_clear_pointer(&model->status_message, g_free);
}
