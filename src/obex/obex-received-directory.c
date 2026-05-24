#include "obex/obex-received-directory.h"

#include <gio/gio.h>

#include "core/user-paths.h"

static char *
obex_received_directory_expand_environment_variables(const char *value)
{
  GString *expanded = g_string_new(NULL);

  for (const char *cursor = value ? value : ""; *cursor;) {
    if (*cursor != '$') {
      g_string_append_c(expanded, *cursor++);
      continue;
    }

    const char *start = cursor;
    cursor++;
    if (*cursor == '{') {
      const char *name_start = ++cursor;

      while (*cursor && *cursor != '}')
        cursor++;
      if (*cursor == '}') {
        g_autofree char *name = g_strndup(name_start, (gsize)(cursor - name_start));
        const char *replacement = g_getenv(name);

        if (replacement)
          g_string_append(expanded, replacement);
        else
          g_string_append_len(expanded, start, (gssize)(cursor - start + 1));
        cursor++;
        continue;
      }

      g_string_append_c(expanded, '$');
      cursor = name_start - 1;
      continue;
    }

    if (g_ascii_isalpha(*cursor) || *cursor == '_') {
      const char *name_start = cursor;

      while (g_ascii_isalnum(*cursor) || *cursor == '_')
        cursor++;

      g_autofree char *name = g_strndup(name_start, (gsize)(cursor - name_start));
      const char *replacement = g_getenv(name);

      if (replacement)
        g_string_append(expanded, replacement);
      else
        g_string_append_len(expanded, start, (gssize)(cursor - start));
      continue;
    }

    g_string_append_c(expanded, '$');
  }

  return g_string_free(expanded, FALSE);
}

char *
obex_received_directory_expand(const char *location)
{
  g_autofree char *stripped = g_strdup(location ? location : "");
  g_autofree char *with_env = NULL;
  char *expanded = NULL;

  g_strstrip(stripped);
  if (!*stripped)
    return paths_default_received_files_location();

  with_env = obex_received_directory_expand_environment_variables(stripped);
  if (g_str_has_prefix(with_env, "~/"))
    expanded = g_build_filename(g_get_home_dir(), with_env + 2, NULL);
  else if (g_str_equal(with_env, "~"))
    expanded = g_strdup(g_get_home_dir());
  else if (g_path_is_absolute(with_env))
    expanded = g_strdup(with_env);
  else
    expanded = g_build_filename(g_get_home_dir(), with_env, NULL);

  return expanded;
}
