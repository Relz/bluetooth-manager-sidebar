#pragma once

#include "settings/settings.h"

typedef struct {
  gboolean settings_available;
  gboolean auto_accept_trusted_files;
  char *received_files_location;
} ApplicationSettingsModel;

ApplicationSettingsModel application_settings_model_new(SidebarSettings *settings);
void application_settings_model_clear(ApplicationSettingsModel *model);
