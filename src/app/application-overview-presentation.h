#pragma once

#include "app/application-overview-model.h"

typedef enum {
  APPLICATION_OVERVIEW_DEVICE_ACTION_TRUST,
  APPLICATION_OVERVIEW_DEVICE_ACTION_SETTINGS,
} OverviewDeviceAction;

typedef struct {
  OverviewDeviceAction action;
  const char *icon_name;
  const char *tooltip;
} OverviewDeviceActionModel;

const char *application_overview_presentation_status_group_title(void);
const char *application_overview_presentation_connected_group_title(void);
const char *application_overview_presentation_paired_group_title(void);
const char *application_overview_presentation_available_group_title(void);
const char *application_overview_presentation_no_adapter_title(void);
const char *application_overview_presentation_discoverable_title(void);
const char *application_overview_presentation_discoverable_subtitle(void);
const char *application_overview_presentation_discoverable_tooltip(void);
const char *application_overview_presentation_blocked_power_title(void);
const char *application_overview_presentation_blocked_power_subtitle(void);
const char *application_overview_presentation_scan_button_label(gboolean discovering);
const char *application_overview_presentation_scan_button_tooltip(void);
const char *application_overview_presentation_available_empty_title(gboolean powered,
                                                                    gboolean discovering);
const char *application_overview_presentation_available_empty_subtitle(gboolean powered,
                                                                       gboolean discovering);
guint application_overview_presentation_device_actions(const OverviewDeviceRowModel *model,
                                                      OverviewDeviceActionModel *actions,
                                                      guint max_actions);
