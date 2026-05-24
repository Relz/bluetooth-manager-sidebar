#include "settings/settings.h"

#include <errno.h>

#include <glib/gstdio.h>

#include "core/config-paths.h"
#include "settings/settings-json-codec.h"

struct _SettingsStore {
  char *path;
  char *legacy_path;
};

SettingsStore *
settings_store_new(const char *path_override)
{
  SettingsStore *store = g_new0(SettingsStore, 1);

  if (path_override && *path_override) {
    store->path = g_strdup(path_override);
  } else {
    store->path = paths_config_path();
    store->legacy_path = paths_legacy_config_path();
  }

  return store;
}

void
settings_store_free(SettingsStore *store)
{
  if (!store)
    return;

  g_free(store->path);
  g_free(store->legacy_path);
  g_free(store);
}

SidebarSettings *
settings_store_load(SettingsStore *store)
{
  g_return_val_if_fail(store != NULL, sidebar_settings_new_default());

  SidebarSettings *settings = sidebar_settings_new_default();
  const char *path = store->path;
  g_autofree char *contents = NULL;
  gsize length = 0;
  g_autoptr(GError) error = NULL;

  if (!g_file_test(path, G_FILE_TEST_EXISTS) && store->legacy_path)
    path = store->legacy_path;

  if (!g_file_get_contents(path, &contents, &length, &error))
    return settings;

  sidebar_settings_free(settings);
  settings = settings_json_codec_parse_data(contents, length);
  return settings;
}

gboolean
settings_store_save(SettingsStore *store,
                    const SidebarSettings *settings,
                    GError **error)
{
  g_return_val_if_fail(store != NULL, FALSE);
  g_return_val_if_fail(settings != NULL, FALSE);

  g_autofree char *parent = g_path_get_dirname(store->path);

  if (g_mkdir_with_parents(parent, 0700) != 0) {
    int saved_errno = errno;
    g_set_error(error,
                G_FILE_ERROR,
                g_file_error_from_errno(saved_errno),
                "Unable to create settings directory %s: %s",
                parent,
                g_strerror(saved_errno));
    return FALSE;
  }

  g_autofree char *data = settings_json_codec_to_data(settings);
  return g_file_set_contents(store->path, data, -1, error);
}
