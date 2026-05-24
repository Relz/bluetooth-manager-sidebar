#include "audio/audio-profile-parser.h"

#include <json-glib/json-glib.h>

#include "audio/audio-profiles-internal.h"

static char *
json_dup_member_string(JsonObject *object, const char *member)
{
  JsonNode *node = object ? json_object_get_member(object, member) : NULL;

  if (!node || !JSON_NODE_HOLDS_VALUE(node))
    return NULL;

  return json_node_dup_string(node);
}

static gboolean
audio_card_matches_address(JsonObject *card, const char *address)
{
  g_autofree char *normalized_address = g_utf8_casefold(address, -1);
  g_autofree char *address_token = g_strdup(address);
  g_autofree char *card_name = json_dup_member_string(card, "name");
  g_autofree char *card_name_lower = card_name ? g_utf8_casefold(card_name, -1) : NULL;
  JsonObject *properties;

  g_strdelimit(address_token, ":", '_');
  char *folded_token = g_utf8_casefold(address_token, -1);
  g_free(address_token);
  address_token = folded_token;

  if (card_name_lower && g_strrstr(card_name_lower, address_token))
    return TRUE;

  properties = json_object_get_object_member(card, "properties");
  if (!properties)
    return FALSE;

  GList *values = json_object_get_values(properties);
  for (GList *node = values; node; node = node->next) {
    JsonNode *value_node = node->data;
    g_autofree char *value = NULL;
    g_autofree char *folded = NULL;

    if (!JSON_NODE_HOLDS_VALUE(value_node))
      continue;
    value = json_node_dup_string(value_node);
    folded = value ? g_utf8_casefold(value, -1) : NULL;
    if (folded && (g_str_equal(folded, normalized_address) || g_strrstr(folded, address_token))) {
      g_list_free(values);
      return TRUE;
    }
  }
  g_list_free(values);

  return FALSE;
}

static gboolean
audio_profile_is_available(JsonObject *profile)
{
  g_autofree char *available = json_dup_member_string(profile, "available");
  g_autofree char *folded = available ? g_utf8_casefold(available, -1) : NULL;

  if (!folded || !*folded)
    return TRUE;
  return !g_str_equal(folded, "no") && !g_str_equal(folded, "false") && !g_str_equal(folded, "0");
}

static void
audio_add_profile_option(AudioProfileInfo *info, const char *name, JsonObject *details)
{
  g_autofree char *description = NULL;

  if (!name || !*name || !details || !audio_profile_is_available(details))
    return;

  description = json_dup_member_string(details, "description");
  g_ptr_array_add(info->profiles, audio_profile_option_new(name, description ? description : name));
}

static void
audio_extract_profiles(AudioProfileInfo *info, JsonObject *card)
{
  JsonNode *profiles = json_object_get_member(card, "profiles");

  if (!profiles)
    return;

  if (JSON_NODE_HOLDS_OBJECT(profiles)) {
    JsonObject *object = json_node_get_object(profiles);
    GList *members = json_object_get_members(object);

    for (GList *node = members; node; node = node->next) {
      const char *name = node->data;
      JsonObject *details = json_object_get_object_member(object, name);

      audio_add_profile_option(info, name, details);
    }
    g_list_free(members);
  } else if (JSON_NODE_HOLDS_ARRAY(profiles)) {
    JsonArray *array = json_node_get_array(profiles);

    for (guint i = 0; i < json_array_get_length(array); i++) {
      JsonNode *node = json_array_get_element(array, i);
      JsonObject *details = JSON_NODE_HOLDS_OBJECT(node) ? json_node_get_object(node) : NULL;
      g_autofree char *name = json_dup_member_string(details, "name");

      audio_add_profile_option(info, name, details);
    }
  }
}

static char *
audio_dup_active_profile(JsonObject *card)
{
  JsonNode *active = json_object_get_member(card, "active_profile");

  if (!active)
    return g_strdup("");
  if (JSON_NODE_HOLDS_OBJECT(active))
    return json_dup_member_string(json_node_get_object(active), "name");
  if (JSON_NODE_HOLDS_VALUE(active))
    return json_node_dup_string(active);
  return g_strdup("");
}

static AudioProfileInfo *
audio_profiles_info_for_card(JsonObject *card)
{
  AudioProfileInfo *info = audio_profile_info_new();

  info->card_name = json_dup_member_string(card, "name");
  info->active_profile = audio_dup_active_profile(card);
  audio_extract_profiles(info, card);
  if (info->profiles->len == 0) {
    audio_profile_info_free(info);
    return audio_profile_info_new_error("missing-profiles", NULL);
  }

  return info;
}

AudioProfileInfo *
audio_profile_parser_parse_cards(const char *stdout_text, const char *address)
{
  g_autoptr(JsonParser) parser = json_parser_new();
  JsonNode *root;

  if (!json_parser_load_from_data(parser, stdout_text ? stdout_text : "[]", -1, NULL))
    return audio_profile_info_new_error("pactl-invalid-json", NULL);

  root = json_parser_get_root(parser);
  if (!root || !JSON_NODE_HOLDS_ARRAY(root))
    return audio_profile_info_new_error("pactl-unexpected-output", NULL);

  JsonArray *cards = json_node_get_array(root);
  for (guint i = 0; i < json_array_get_length(cards); i++) {
    JsonNode *node = json_array_get_element(cards, i);
    JsonObject *card = JSON_NODE_HOLDS_OBJECT(node) ? json_node_get_object(node) : NULL;

    if (card && audio_card_matches_address(card, address))
      return audio_profiles_info_for_card(card);
  }

  return audio_profile_info_new_error("missing-card", NULL);
}
