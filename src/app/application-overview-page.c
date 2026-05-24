#include "app/application-overview-page.h"

#include "app/application-overview-page-model.h"
#include "app/application-overview-presenter.h"

void
application_overview_page_build(Application *self)
{
  ApplicationOverviewPageModel page_model = application_overview_page_model_new(self);

  application_overview_presenter_render(self, &page_model);
  application_overview_page_model_clear(&page_model);
}
