#include "core/target-output.h"

#include <string.h>

static char *
utf8_strip_copy(const char *value)
{
  const char *start;
  const char *end;

  if (!value || !g_utf8_validate(value, -1, NULL))
    return NULL;

  start = value;
  end = value + strlen(value);

  while (start < end) {
    gunichar ch = g_utf8_get_char(start);
    if (!g_unichar_isspace(ch))
      break;
    start = g_utf8_next_char(start);
  }

  while (end > start) {
    const char *previous = g_utf8_prev_char(end);
    gunichar ch = g_utf8_get_char(previous);
    if (!g_unichar_isspace(ch))
      break;
    end = previous;
  }

  return g_strndup(start, end - start);
}

char *
target_output_normalize(const char *output_name)
{
  g_autofree char *trimmed = NULL;

  if (!output_name)
    return NULL;

  trimmed = utf8_strip_copy(output_name);
  if (!trimmed || !*trimmed)
    return NULL;
  if (strchr(trimmed, '\n') || strchr(trimmed, '\t'))
    return NULL;
  if (g_utf8_strlen(trimmed, -1) > TARGET_OUTPUT_MAX_CHARS)
    return NULL;

  return g_strdup(trimmed);
}

char *
target_output_from_environment(void)
{
  static const char *env_vars[] = {
    "BM_SIDEBAR_OUTPUT",
    "WAYBAR_OUTPUT_NAME",
    NULL,
  };

  for (guint i = 0; env_vars[i]; i++) {
    g_autofree char *normalized = target_output_normalize(g_getenv(env_vars[i]));
    if (normalized)
      return g_steal_pointer(&normalized);
  }

  return NULL;
}
