#pragma once

#include "app/application-overview-model.h"

typedef struct {
  BluezClient *bluez;
  OverviewModel model;
} ApplicationOverviewPageModel;

ApplicationOverviewPageModel application_overview_page_model_new(Application *self);
void application_overview_page_model_clear(ApplicationOverviewPageModel *page_model);
