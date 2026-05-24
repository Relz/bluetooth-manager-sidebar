#include "core/asset-paths.h"

#include <glib.h>

#include "core/config.h"

char *
paths_css_path(void)
{
  g_autofree char *dev_path = g_build_filename(SOURCE_DIR, "bm-sidebar.css", NULL);

  if (g_file_test(dev_path, G_FILE_TEST_IS_REGULAR))
    return g_steal_pointer(&dev_path);
  if (g_file_test(INSTALLED_CSS_PATH, G_FILE_TEST_IS_REGULAR))
    return g_strdup(INSTALLED_CSS_PATH);
  return g_steal_pointer(&dev_path);
}

char *
paths_user_css_path(void)
{
  return g_build_filename(g_get_user_config_dir(), COMMAND_NAME, "bm-sidebar.css", NULL);
}
