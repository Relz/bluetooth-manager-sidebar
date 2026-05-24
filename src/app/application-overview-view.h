#pragma once

#include "app/application-overview-model.h"

GtkWidget *application_overview_view_device_row(const OverviewDeviceRowModel *model);
GtkWidget *application_overview_view_discoverable_row(gboolean discoverable,
                                                      gboolean powered,
                                                      GtkWidget **out_toggle);
GtkWidget *application_overview_view_scan_button(gboolean discovering);
GtkWidget *application_overview_view_status_group(void);
GtkWidget *application_overview_view_device_group(const char *title);
void application_overview_view_add_status_rows(const OverviewStatusModel *status,
                                               AdwPreferencesGroup *group);
void application_overview_view_add_blocked_power_row(AdwPreferencesGroup *status_group,
                                                    const OverviewModel *model);
void application_overview_view_add_available_empty_row(AdwPreferencesGroup *group,
                                                       gboolean powered,
                                                       gboolean discovering);
void application_overview_view_add_no_adapter_row(AdwPreferencesGroup *group,
                                                 const char *message);
