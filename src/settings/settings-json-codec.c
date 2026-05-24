#include "settings/settings-json-codec.h"

#include <json-glib/json-glib.h>

static gboolean
json_node_is_boolean(JsonNode *node)
{
  return node && JSON_NODE_HOLDS_VALUE(node) && json_node_get_value_type(node) == G_TYPE_BOOLEAN;
}

static gboolean
json_node_is_string(JsonNode *node)
{
  return node && JSON_NODE_HOLDS_VALUE(node) && json_node_get_value_type(node) == G_TYPE_STRING;
}

SidebarSettings *
settings_json_codec_parse_data(const char *contents, gsize length)
{
  SidebarSettings *settings = sidebar_settings_new_default();
  g_autoptr(GError) error = NULL;
  g_autoptr(JsonParser) parser = json_parser_new();
  JsonNode *root;
  JsonObject *object;
  JsonNode *auto_accept;
  JsonNode *location;

  if (!contents || !json_parser_load_from_data(parser, contents, (gssize)length, &error))
    return settings;

  root = json_parser_get_root(parser);
  if (!root || !JSON_NODE_HOLDS_OBJECT(root))
    return settings;

  object = json_node_get_object(root);
  auto_accept = json_object_get_member(object, "auto_accept_trusted_files");
  location = json_object_get_member(object, "received_files_location");

  if (json_node_is_boolean(auto_accept))
    sidebar_settings_set_auto_accept_trusted_files(settings, json_node_get_boolean(auto_accept));

  if (json_node_is_string(location))
    sidebar_settings_set_received_files_location(settings, json_node_get_string(location));

  return settings;
}

char *
settings_json_codec_to_data(const SidebarSettings *settings)
{
  const char *received_files_location = sidebar_settings_get_received_files_location(settings);
  g_autoptr(JsonBuilder) builder = json_builder_new();
  g_autoptr(JsonNode) root = NULL;
  g_autoptr(JsonGenerator) generator = json_generator_new();
  g_autofree char *data = NULL;

  json_builder_begin_object(builder);
  json_builder_set_member_name(builder, "auto_accept_trusted_files");
  json_builder_add_boolean_value(builder, sidebar_settings_get_auto_accept_trusted_files(settings));
  json_builder_set_member_name(builder, "received_files_location");
  json_builder_add_string_value(builder, received_files_location ? received_files_location : "");
  json_builder_end_object(builder);

  root = json_builder_get_root(builder);
  json_generator_set_pretty(generator, TRUE);
  json_generator_set_indent(generator, 2);
  json_generator_set_root(generator, root);

  data = json_generator_to_data(generator, NULL);
  return g_strconcat(data ? data : "{}", "\n", NULL);
}
