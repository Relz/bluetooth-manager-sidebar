#pragma once

#include "app/application-overview-model.h"

void application_overview_actions_bind_device_row(Application *self,
                                                  GtkWidget *row,
                                                  const char *device_path,
                                                  const OverviewDeviceRowModel *model);
void application_overview_actions_bind_discoverable_toggle(Application *self,
                                                           GtkWidget *toggle);
void application_overview_actions_bind_scan_button(Application *self,
                                                   GtkWidget *button);
