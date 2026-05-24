#include "core/user-paths.h"

#include <glib.h>

char *
paths_default_received_files_location(void)
{
  return g_build_filename(g_get_home_dir(), "Downloads", NULL);
}
