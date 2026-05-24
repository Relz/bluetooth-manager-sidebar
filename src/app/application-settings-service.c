#include "app/application-settings-service.h"

static gboolean
application_settings_service_save(SettingsStore *store,
                                  SidebarSettings *settings,
                                  GError **error)
{
  if (!store || !settings)
    return FALSE;

  return settings_store_save(store, settings, error);
}

gboolean
application_settings_service_set_auto_accept_trusted_files(SettingsStore *store,
                                                           SidebarSettings *settings,
                                                           gboolean auto_accept,
                                                           GError **error)
{
  g_autoptr(SidebarSettings) candidate = NULL;

  if (!store || !settings)
    return FALSE;

  candidate = sidebar_settings_copy(settings);
  sidebar_settings_set_auto_accept_trusted_files(candidate, auto_accept);
  if (!application_settings_service_save(store, candidate, error))
    return FALSE;

  sidebar_settings_set_auto_accept_trusted_files(settings, auto_accept);
  return TRUE;
}

gboolean
application_settings_service_set_received_files_location(SettingsStore *store,
                                                        SidebarSettings *settings,
                                                        const char *location,
                                                        gboolean *out_changed,
                                                        GError **error)
{
  const char *current = NULL;
  g_autofree char *normalized = NULL;

  if (out_changed)
    *out_changed = FALSE;
  if (!store || !settings)
    return FALSE;

  current = sidebar_settings_get_received_files_location(settings);
  normalized = sidebar_settings_dup_normalized_received_files_location(location);
  if (g_strcmp0(normalized, current ? current : "") == 0)
    return TRUE;

  g_autoptr(SidebarSettings) candidate = sidebar_settings_copy(settings);
  sidebar_settings_set_received_files_location(candidate, normalized);
  if (!application_settings_service_save(store, candidate, error))
    return FALSE;

  sidebar_settings_set_received_files_location(settings, normalized);
  if (out_changed)
    *out_changed = TRUE;
  return TRUE;
}
