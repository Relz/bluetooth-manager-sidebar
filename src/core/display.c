#include "core/display.h"

#include <string.h>

static void
append_codepoint_escape(GString *result, gunichar ch)
{
  if (ch <= 0xff)
    g_string_append_printf(result, "\\x%02x", ch);
  else if (ch <= 0xffff)
    g_string_append_printf(result, "\\u%04x", ch);
  else
    g_string_append_printf(result, "\\U%08x", ch);
}

static gboolean
unicode_category_hidden(GUnicodeType type)
{
  return type == G_UNICODE_CONTROL ||
         type == G_UNICODE_FORMAT ||
         type == G_UNICODE_UNASSIGNED ||
         type == G_UNICODE_PRIVATE_USE ||
         type == G_UNICODE_SURROGATE ||
         type == G_UNICODE_LINE_SEPARATOR ||
         type == G_UNICODE_PARAGRAPH_SEPARATOR;
}

static char *
display_sanitize_nonempty_text(const char *value)
{
  g_autoptr(GString) result = NULL;
  const char *end;
  const char *first_non_space = NULL;
  const char *last_non_space = NULL;

  if (!value || !*value || !g_utf8_validate(value, -1, NULL))
    return NULL;

  end = value + strlen(value);
  for (const char *p = value; p < end; p = g_utf8_next_char(p)) {
    gunichar ch = g_utf8_get_char(p);

    if (!g_unichar_isspace(ch)) {
      if (!first_non_space)
        first_non_space = p;
      last_non_space = p;
    }
  }

  if (!first_non_space)
    return NULL;

  result = g_string_new(NULL);
  for (const char *p = value; p < end;) {
    const char *next = g_utf8_next_char(p);
    gunichar ch = g_utf8_get_char(p);
    GUnicodeType type = g_unichar_type(ch);
    gboolean edge_space = g_unichar_isspace(ch) && (p < first_non_space || p > last_non_space);

    if (ch == '\n')
      g_string_append(result, "\\n");
    else if (ch == '\r')
      g_string_append(result, "\\r");
    else if (ch == '\t')
      g_string_append(result, "\\t");
    else if (edge_space || unicode_category_hidden(type))
      append_codepoint_escape(result, ch);
    else
      g_string_append_len(result, p, next - p);

    p = next;
  }

  return g_string_free(g_steal_pointer(&result), FALSE);
}

char *
format_size(guint64 size)
{
  const char *units[] = { "B", "KB", "MB", "GB" };
  double amount = (double)size;

  for (guint i = 0; i < G_N_ELEMENTS(units); i++) {
    if (amount < 1024.0 || i == G_N_ELEMENTS(units) - 1) {
      if (i == 0)
        return g_strdup_printf("%" G_GUINT64_FORMAT " %s", size, units[i]);

      char buffer[G_ASCII_DTOSTR_BUF_SIZE];
      g_ascii_formatd(buffer, sizeof buffer, "%.1f", amount);
      return g_strdup_printf("%s %s", buffer, units[i]);
    }
    amount /= 1024.0;
  }

  return NULL;
}

char *
display_dup_safe_text(const char *value, const char *fallback)
{
  g_autofree char *safe = display_sanitize_nonempty_text(value);

  if (safe)
    return g_steal_pointer(&safe);

  return display_sanitize_nonempty_text(fallback);
}

char *
device_display_name(const char *alias,
                         const char *name,
                         const char *address,
                         const char *fallback)
{
  g_autofree char *safe = display_dup_safe_text(alias, NULL);

  if (safe)
    return g_steal_pointer(&safe);
  safe = display_dup_safe_text(name, NULL);
  if (safe)
    return g_steal_pointer(&safe);
  safe = display_dup_safe_text(address, NULL);
  if (safe)
    return g_steal_pointer(&safe);
  return display_dup_safe_text(fallback, "Unknown device");
}
