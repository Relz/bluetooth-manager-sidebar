#pragma once

#include <glib.h>

typedef struct {
  gboolean auto_accept_trusted_files;
  char *received_files_location;
} SidebarSettings;

typedef struct _SettingsStore SettingsStore;

SidebarSettings *sidebar_settings_new_default(void);
SidebarSettings *sidebar_settings_copy(const SidebarSettings *settings);
void sidebar_settings_free(SidebarSettings *settings);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(SidebarSettings, sidebar_settings_free)
gboolean sidebar_settings_get_auto_accept_trusted_files(const SidebarSettings *settings);
const char *sidebar_settings_get_received_files_location(const SidebarSettings *settings);
void sidebar_settings_set_auto_accept_trusted_files(SidebarSettings *settings,
                                                        gboolean enabled);
char *sidebar_settings_dup_normalized_received_files_location(const char *location);
void sidebar_settings_set_received_files_location(SidebarSettings *settings,
                                                      const char *location);

SettingsStore *settings_store_new(const char *path_override);
void settings_store_free(SettingsStore *store);

SidebarSettings *settings_store_load(SettingsStore *store);
gboolean settings_store_save(SettingsStore *store,
                                 const SidebarSettings *settings,
                                 GError **error);
