#pragma once

#include "settings/settings.h"

typedef struct _ApplicationSettingsRuntime ApplicationSettingsRuntime;

ApplicationSettingsRuntime *application_settings_runtime_new(void);
void application_settings_runtime_free(ApplicationSettingsRuntime *runtime);

SettingsStore *application_settings_runtime_store(ApplicationSettingsRuntime *runtime);
SidebarSettings *application_settings_runtime_settings(ApplicationSettingsRuntime *runtime);
