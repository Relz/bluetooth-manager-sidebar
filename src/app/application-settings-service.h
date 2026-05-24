#pragma once

#include "settings/settings.h"

gboolean application_settings_service_set_auto_accept_trusted_files(SettingsStore *store,
                                                                   SidebarSettings *settings,
                                                                   gboolean auto_accept,
                                                                   GError **error);
gboolean application_settings_service_set_received_files_location(SettingsStore *store,
                                                                 SidebarSettings *settings,
                                                                 const char *location,
                                                                 gboolean *out_changed,
                                                                 GError **error);
