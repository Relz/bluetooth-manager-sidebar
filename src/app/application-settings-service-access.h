#pragma once

#include "app/application.h"
#include "settings/settings.h"

SettingsStore *application_services_settings_store(Application *self);
SidebarSettings *application_services_settings(Application *self);
