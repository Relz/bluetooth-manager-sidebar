#include "app/application-overview-page-model.h"

#include "app/application-bluez-service-access.h"
#include "app/application-obex-service-access.h"

ApplicationOverviewPageModel
application_overview_page_model_new(Application *self)
{
  BluezClient *bluez = application_services_bluez_client(self);

  return (ApplicationOverviewPageModel){
    .bluez = bluez,
    .model = application_overview_model_new(self,
                                            bluez,
                                            application_services_obex_service(self)),
  };
}

void
application_overview_page_model_clear(ApplicationOverviewPageModel *page_model)
{
  application_overview_model_clear(&page_model->model);
}
