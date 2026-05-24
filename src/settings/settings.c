#include "settings/settings.h"

#include "core/user-paths.h"

SidebarSettings *
sidebar_settings_new_default(void)
{
  SidebarSettings *settings = g_new0(SidebarSettings, 1);

  settings->auto_accept_trusted_files = FALSE;
  settings->received_files_location = paths_default_received_files_location();

  return settings;
}

SidebarSettings *
sidebar_settings_copy(const SidebarSettings *settings)
{
  SidebarSettings *copy = NULL;

  if (!settings)
    return NULL;

  copy = g_new0(SidebarSettings, 1);
  copy->auto_accept_trusted_files = settings->auto_accept_trusted_files;
  copy->received_files_location = g_strdup(settings->received_files_location);
  return copy;
}

void
sidebar_settings_free(SidebarSettings *settings)
{
  if (!settings)
    return;

  g_free(settings->received_files_location);
  g_free(settings);
}

gboolean
sidebar_settings_get_auto_accept_trusted_files(const SidebarSettings *settings)
{
  return settings ? settings->auto_accept_trusted_files : FALSE;
}

const char *
sidebar_settings_get_received_files_location(const SidebarSettings *settings)
{
  return settings ? settings->received_files_location : NULL;
}

void
sidebar_settings_set_auto_accept_trusted_files(SidebarSettings *settings, gboolean enabled)
{
  if (settings)
    settings->auto_accept_trusted_files = enabled;
}

char *
sidebar_settings_dup_normalized_received_files_location(const char *location)
{
  g_autofree char *stripped = g_strdup(location ? location : "");

  g_strstrip(stripped);
  return *stripped ? g_strdup(stripped) : paths_default_received_files_location();
}

void
sidebar_settings_set_received_files_location(SidebarSettings *settings, const char *location)
{
  if (!settings)
    return;

  g_free(settings->received_files_location);
  settings->received_files_location = sidebar_settings_dup_normalized_received_files_location(location);
}
