#include "bluez/bluez-object-path.h"

#define BLUEZ_ROOT_PATH "/org/bluez"

gboolean
bluez_object_path_is_valid(const char *path)
{
  return path &&
         (g_str_equal(path, BLUEZ_ROOT_PATH) ||
          g_str_has_prefix(path, BLUEZ_ROOT_PATH "/"));
}
