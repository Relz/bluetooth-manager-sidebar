#include "core/config-paths.h"

#include <glib.h>

#include "core/config.h"

static char *
config_home(void)
{
  const char *xdg_config_home = g_getenv("XDG_CONFIG_HOME");

  if (xdg_config_home && *xdg_config_home)
    return g_strdup(xdg_config_home);

  return g_build_filename(g_get_home_dir(), ".config", NULL);
}

char *
paths_config_path(void)
{
  g_autofree char *config_dir = config_home();

  return g_build_filename(config_dir, COMMAND_NAME, "settings.json", NULL);
}

char *
paths_legacy_config_path(void)
{
  g_autofree char *config_dir = config_home();

  return g_build_filename(config_dir, "bluetooth-sidebar", "settings.json", NULL);
}
