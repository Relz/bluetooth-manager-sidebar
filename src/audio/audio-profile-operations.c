#include "audio/audio-profile-operations.h"

#include <errno.h>
#include <gio/gio.h>
#include <json-glib/json-glib.h>
#include <string.h>

#define AUDIO_ROUTE_RESTORE_ATTEMPTS 15
#define AUDIO_ROUTE_RESTORE_DELAY_USEC (100 * 1000)
#define AUDIO_MEDIA_DBUS_TIMEOUT_MS 1000
#define AUDIO_MEDIA_RESUME_ATTEMPTS 8
#define AUDIO_MEDIA_RESUME_DELAY_USEC (250 * 1000)
#define DBUS_BUS_NAME "org.freedesktop.DBus"
#define DBUS_OBJECT_PATH "/org/freedesktop/DBus"
#define DBUS_INTERFACE "org.freedesktop.DBus"
#define DBUS_PROPERTIES_INTERFACE "org.freedesktop.DBus.Properties"
#define MPRIS_BUS_NAME_PREFIX "org.mpris.MediaPlayer2."
#define MPRIS_OBJECT_PATH "/org/mpris/MediaPlayer2"
#define MPRIS_PLAYER_INTERFACE "org.mpris.MediaPlayer2.Player"

typedef struct {
  char *default_sink;
  GPtrArray *sink_names;
  GArray *sink_indexes;
  GArray *sink_input_indexes;
  gboolean default_was_target;
} AudioRouteSnapshot;

typedef struct {
  GDBusConnection *connection;
  GPtrArray *player_names;
} MediaPlaybackSnapshot;

static AudioRouteSnapshot *
audio_route_snapshot_new(void)
{
  AudioRouteSnapshot *snapshot = g_new0(AudioRouteSnapshot, 1);

  snapshot->sink_names = g_ptr_array_new_with_free_func(g_free);
  snapshot->sink_indexes = g_array_new(FALSE, FALSE, sizeof(guint));
  snapshot->sink_input_indexes = g_array_new(FALSE, FALSE, sizeof(guint));
  return snapshot;
}

static void
audio_route_snapshot_free(AudioRouteSnapshot *snapshot)
{
  if (!snapshot)
    return;

  g_free(snapshot->default_sink);
  g_ptr_array_unref(snapshot->sink_names);
  g_array_unref(snapshot->sink_indexes);
  g_array_unref(snapshot->sink_input_indexes);
  g_free(snapshot);
}

static MediaPlaybackSnapshot *
audio_media_playback_snapshot_new(GDBusConnection *connection)
{
  MediaPlaybackSnapshot *snapshot = g_new0(MediaPlaybackSnapshot, 1);

  snapshot->connection = g_object_ref(connection);
  snapshot->player_names = g_ptr_array_new_with_free_func(g_free);
  return snapshot;
}

static void
audio_media_playback_snapshot_free(MediaPlaybackSnapshot *snapshot)
{
  if (!snapshot)
    return;

  g_clear_object(&snapshot->connection);
  g_ptr_array_unref(snapshot->player_names);
  g_free(snapshot);
}

static gboolean
audio_json_parser_load_array(JsonParser *parser, const char *stdout_text, JsonArray **out_array)
{
  JsonNode *root;

  if (!json_parser_load_from_data(parser, stdout_text ? stdout_text : "[]", -1, NULL))
    return FALSE;

  root = json_parser_get_root(parser);
  if (!root || !JSON_NODE_HOLDS_ARRAY(root))
    return FALSE;

  *out_array = json_node_get_array(root);
  return TRUE;
}

static char *
audio_json_dup_member_string(JsonObject *object, const char *member)
{
  JsonNode *node = object ? json_object_get_member(object, member) : NULL;

  if (!node || !JSON_NODE_HOLDS_VALUE(node))
    return NULL;

  return json_node_dup_string(node);
}

static gboolean
audio_json_parse_uint_string(const char *text, guint *out_value)
{
  g_autofree char *copy = g_strdup(text);
  char *end = NULL;
  guint64 value;

  if (!copy)
    return FALSE;

  g_strstrip(copy);
  if (!*copy)
    return FALSE;

  errno = 0;
  value = g_ascii_strtoull(copy, &end, 10);
  if (errno != 0 || end == copy || *end != '\0' || value > G_MAXUINT)
    return FALSE;

  *out_value = (guint)value;
  return TRUE;
}

static gboolean
audio_json_get_member_uint(JsonObject *object, const char *member, guint *out_value)
{
  JsonNode *node = object ? json_object_get_member(object, member) : NULL;
  GType value_type;
  gint64 value;

  if (!node || !JSON_NODE_HOLDS_VALUE(node))
    return FALSE;

  value_type = json_node_get_value_type(node);
  if (value_type == G_TYPE_STRING) {
    g_autofree char *text = json_node_dup_string(node);

    return audio_json_parse_uint_string(text, out_value);
  }

  if (value_type != G_TYPE_INT64 && value_type != G_TYPE_INT && value_type != G_TYPE_UINT &&
      value_type != G_TYPE_UINT64 && value_type != G_TYPE_LONG && value_type != G_TYPE_ULONG)
    return FALSE;

  value = json_node_get_int(node);
  if (value < 0 || value > G_MAXUINT)
    return FALSE;

  *out_value = (guint)value;
  return TRUE;
}

static gboolean
audio_route_uint_array_contains(GArray *array, guint value)
{
  for (guint i = 0; array && i < array->len; i++) {
    guint item = g_array_index(array, guint, i);

    if (item == value)
      return TRUE;
  }
  return FALSE;
}

static gboolean
audio_route_sink_names_contain(GPtrArray *names, const char *value)
{
  if (!value || !*value)
    return FALSE;

  for (guint i = 0; names && i < names->len; i++) {
    const char *name = g_ptr_array_index(names, i);

    if (g_strcmp0(name, value) == 0)
      return TRUE;
  }
  return FALSE;
}

static gboolean
audio_route_sink_belongs_to_card(JsonObject *sink, const char *card_name)
{
  JsonObject *properties = json_object_get_object_member(sink, "properties");
  g_autofree char *device_name = audio_json_dup_member_string(properties, "device.name");

  return card_name && *card_name && g_strcmp0(device_name, card_name) == 0;
}

static gboolean
audio_route_input_targets_snapshot(AudioRouteSnapshot *snapshot, JsonObject *input)
{
  guint sink_index = 0;
  g_autofree char *sink_name = NULL;

  if (audio_json_get_member_uint(input, "sink", &sink_index) &&
      audio_route_uint_array_contains(snapshot->sink_indexes, sink_index))
    return TRUE;

  sink_name = audio_json_dup_member_string(input, "sink");
  return audio_route_sink_names_contain(snapshot->sink_names, sink_name);
}

static void
audio_route_capture_target_sinks(AudioRouteSnapshot *snapshot,
                                 const char *stdout_text,
                                 const char *card_name)
{
  g_autoptr(JsonParser) parser = json_parser_new();
  JsonArray *sinks = NULL;

  if (!audio_json_parser_load_array(parser, stdout_text, &sinks))
    return;

  for (guint i = 0; i < json_array_get_length(sinks); i++) {
    JsonNode *node = json_array_get_element(sinks, i);
    JsonObject *sink = JSON_NODE_HOLDS_OBJECT(node) ? json_node_get_object(node) : NULL;
    g_autofree char *name = NULL;
    guint index = 0;

    if (!sink || !audio_route_sink_belongs_to_card(sink, card_name))
      continue;

    name = audio_json_dup_member_string(sink, "name");
    if (name && *name)
      g_ptr_array_add(snapshot->sink_names, g_steal_pointer(&name));
    if (audio_json_get_member_uint(sink, "index", &index))
      g_array_append_val(snapshot->sink_indexes, index);
  }
}

static void
audio_route_capture_sink_inputs(AudioRouteSnapshot *snapshot, const char *stdout_text)
{
  g_autoptr(JsonParser) parser = json_parser_new();
  JsonArray *inputs = NULL;

  if (!audio_json_parser_load_array(parser, stdout_text, &inputs))
    return;

  for (guint i = 0; i < json_array_get_length(inputs); i++) {
    JsonNode *node = json_array_get_element(inputs, i);
    JsonObject *input = JSON_NODE_HOLDS_OBJECT(node) ? json_node_get_object(node) : NULL;
    guint index = 0;

    if (!input || !audio_json_get_member_uint(input, "index", &index))
      continue;

    if (audio_route_input_targets_snapshot(snapshot, input))
      g_array_append_val(snapshot->sink_input_indexes, index);
  }
}

static AudioRouteSnapshot *
audio_route_snapshot_capture(const char *card_name)
{
  g_autofree char *sinks_json = NULL;
  g_autofree char *sink_inputs_json = NULL;
  PactlError *error = NULL;
  AudioRouteSnapshot *snapshot;

  if (!card_name || !*card_name)
    return NULL;

  snapshot = audio_route_snapshot_new();
  if (pactl_list_sinks_json(&sinks_json, &error))
    audio_route_capture_target_sinks(snapshot, sinks_json, card_name);
  g_clear_pointer(&error, pactl_error_free);

  if (snapshot->sink_names->len == 0 && snapshot->sink_indexes->len == 0)
    return snapshot;

  error = pactl_get_default_sink(&snapshot->default_sink);
  g_clear_pointer(&error, pactl_error_free);
  snapshot->default_was_target = audio_route_sink_names_contain(snapshot->sink_names,
                                                               snapshot->default_sink);

  if (snapshot->sink_indexes->len > 0 && pactl_list_sink_inputs_json(&sink_inputs_json, &error))
    audio_route_capture_sink_inputs(snapshot, sink_inputs_json);
  g_clear_pointer(&error, pactl_error_free);

  return snapshot;
}

static gboolean
audio_profile_likely_has_output_sink(const char *profile_name)
{
  if (!profile_name || !*profile_name || g_strcmp0(profile_name, "off") == 0)
    return FALSE;

  return !g_str_has_prefix(profile_name, "input:");
}

static gboolean
audio_route_snapshot_needs_restore(AudioRouteSnapshot *snapshot)
{
  return snapshot && (snapshot->default_was_target || snapshot->sink_input_indexes->len > 0);
}

static gboolean
audio_media_name_is_mpris_player(const char *name)
{
  const gsize prefix_len = strlen(MPRIS_BUS_NAME_PREFIX);

  return name && g_str_has_prefix(name, MPRIS_BUS_NAME_PREFIX) && name[prefix_len] != '\0';
}

static char **
audio_media_dup_bus_names(GDBusConnection *connection)
{
  g_autoptr(GVariant) reply = NULL;
  g_autoptr(GVariant) names = NULL;

  if (!connection)
    return NULL;

  reply = g_dbus_connection_call_sync(connection,
                                      DBUS_BUS_NAME,
                                      DBUS_OBJECT_PATH,
                                      DBUS_INTERFACE,
                                      "ListNames",
                                      NULL,
                                      G_VARIANT_TYPE("(as)"),
                                      G_DBUS_CALL_FLAGS_NONE,
                                      AUDIO_MEDIA_DBUS_TIMEOUT_MS,
                                      NULL,
                                      NULL);
  if (!reply)
    return NULL;

  names = g_variant_get_child_value(reply, 0);
  if (!g_variant_is_of_type(names, G_VARIANT_TYPE("as")))
    return NULL;

  return g_variant_dup_strv(names, NULL);
}

static char *
audio_media_dup_playback_status(GDBusConnection *connection, const char *player_name)
{
  g_autoptr(GVariant) reply = NULL;
  g_autoptr(GVariant) value = NULL;
  g_autoptr(GVariant) unboxed = NULL;

  if (!connection || !player_name || !*player_name)
    return NULL;

  reply = g_dbus_connection_call_sync(connection,
                                      player_name,
                                      MPRIS_OBJECT_PATH,
                                      DBUS_PROPERTIES_INTERFACE,
                                      "Get",
                                      g_variant_new("(ss)",
                                                    MPRIS_PLAYER_INTERFACE,
                                                    "PlaybackStatus"),
                                      G_VARIANT_TYPE("(v)"),
                                      G_DBUS_CALL_FLAGS_NONE,
                                      AUDIO_MEDIA_DBUS_TIMEOUT_MS,
                                      NULL,
                                      NULL);
  if (!reply)
    return NULL;

  value = g_variant_get_child_value(reply, 0);
  if (!g_variant_is_of_type(value, G_VARIANT_TYPE_VARIANT))
    return NULL;

  unboxed = g_variant_get_variant(value);
  if (!g_variant_is_of_type(unboxed, G_VARIANT_TYPE_STRING))
    return NULL;

  return g_strdup(g_variant_get_string(unboxed, NULL));
}

static gboolean
audio_media_player_is_playing(GDBusConnection *connection, const char *player_name)
{
  g_autofree char *status = audio_media_dup_playback_status(connection, player_name);

  return g_strcmp0(status, "Playing") == 0;
}

static gboolean
audio_media_resume_player_if_needed(GDBusConnection *connection, const char *player_name)
{
  GVariant *reply = NULL;

  if (audio_media_player_is_playing(connection, player_name))
    return TRUE;

  reply = g_dbus_connection_call_sync(connection,
                                      player_name,
                                      MPRIS_OBJECT_PATH,
                                      MPRIS_PLAYER_INTERFACE,
                                      "Play",
                                      NULL,
                                      NULL,
                                      G_DBUS_CALL_FLAGS_NONE,
                                      AUDIO_MEDIA_DBUS_TIMEOUT_MS,
                                      NULL,
                                      NULL);
  if (reply)
    g_variant_unref(reply);

  return FALSE;
}

static MediaPlaybackSnapshot *
audio_media_playback_snapshot_capture(AudioRouteSnapshot *route_snapshot,
                                      const char *profile_name)
{
  g_autoptr(GDBusConnection) connection = NULL;
  g_auto(GStrv) names = NULL;
  MediaPlaybackSnapshot *snapshot;

  if (!audio_route_snapshot_needs_restore(route_snapshot) ||
      !audio_profile_likely_has_output_sink(profile_name))
    return NULL;

  connection = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, NULL);
  if (!connection)
    return NULL;

  names = audio_media_dup_bus_names(connection);
  if (!names)
    return NULL;

  snapshot = audio_media_playback_snapshot_new(connection);
  for (char **name = names; *name; name++) {
    if (audio_media_name_is_mpris_player(*name) &&
        audio_media_player_is_playing(connection, *name))
      g_ptr_array_add(snapshot->player_names, g_strdup(*name));
  }

  if (snapshot->player_names->len == 0) {
    audio_media_playback_snapshot_free(snapshot);
    return NULL;
  }

  return snapshot;
}

static void
audio_media_playback_snapshot_resume(MediaPlaybackSnapshot *snapshot)
{
  if (!snapshot || !snapshot->connection || snapshot->player_names->len == 0)
    return;

  g_usleep(AUDIO_MEDIA_RESUME_DELAY_USEC);
  for (guint attempt = 0; attempt < AUDIO_MEDIA_RESUME_ATTEMPTS; attempt++) {
    for (guint i = 0; i < snapshot->player_names->len; i++) {
      const char *player_name = g_ptr_array_index(snapshot->player_names, i);

      audio_media_resume_player_if_needed(snapshot->connection, player_name);
    }

    if (attempt + 1 < AUDIO_MEDIA_RESUME_ATTEMPTS)
      g_usleep(AUDIO_MEDIA_RESUME_DELAY_USEC);
  }
}

static char *
audio_route_dup_first_sink_for_card(const char *stdout_text, const char *card_name)
{
  g_autoptr(JsonParser) parser = json_parser_new();
  JsonArray *sinks = NULL;

  if (!audio_json_parser_load_array(parser, stdout_text, &sinks))
    return NULL;

  for (guint i = 0; i < json_array_get_length(sinks); i++) {
    JsonNode *node = json_array_get_element(sinks, i);
    JsonObject *sink = JSON_NODE_HOLDS_OBJECT(node) ? json_node_get_object(node) : NULL;
    g_autofree char *name = NULL;

    if (!sink || !audio_route_sink_belongs_to_card(sink, card_name))
      continue;

    name = audio_json_dup_member_string(sink, "name");
    if (name && *name)
      return g_steal_pointer(&name);
  }

  return NULL;
}

static char *
audio_route_dup_current_sink_for_card_with_retry(const char *card_name)
{
  for (guint attempt = 0; attempt < AUDIO_ROUTE_RESTORE_ATTEMPTS; attempt++) {
    g_autofree char *sinks_json = NULL;
    PactlError *error = NULL;

    if (pactl_list_sinks_json(&sinks_json, &error)) {
      g_autofree char *sink_name = audio_route_dup_first_sink_for_card(sinks_json, card_name);

      if (sink_name)
        return g_steal_pointer(&sink_name);
    }
    g_clear_pointer(&error, pactl_error_free);

    if (attempt + 1 < AUDIO_ROUTE_RESTORE_ATTEMPTS)
      g_usleep(AUDIO_ROUTE_RESTORE_DELAY_USEC);
  }

  return NULL;
}

static void
audio_route_snapshot_restore(AudioRouteSnapshot *snapshot,
                             const char *card_name,
                             const char *profile_name)
{
  g_autofree char *sink_name = NULL;

  if (!audio_route_snapshot_needs_restore(snapshot) ||
      !audio_profile_likely_has_output_sink(profile_name))
    return;

  sink_name = audio_route_dup_current_sink_for_card_with_retry(card_name);
  if (!sink_name)
    return;

  if (snapshot->default_was_target)
    pactl_error_free(pactl_set_default_sink(sink_name));

  for (guint i = 0; i < snapshot->sink_input_indexes->len; i++) {
    guint input_index = g_array_index(snapshot->sink_input_indexes, guint, i);

    pactl_error_free(pactl_move_sink_input(input_index, sink_name));
  }
}

gboolean
audio_profile_operations_load_cards_json(char **stdout_text, PactlError **out_error)
{
  return pactl_list_cards_json(stdout_text, out_error);
}

PactlError *
audio_profile_operations_set_profile(const char *card_name, const char *profile_name)
{
  AudioRouteSnapshot *snapshot = audio_route_snapshot_capture(card_name);
  MediaPlaybackSnapshot *playback_snapshot =
    audio_media_playback_snapshot_capture(snapshot, profile_name);
  PactlError *error = pactl_set_card_profile(card_name, profile_name);

  if (!error) {
    audio_route_snapshot_restore(snapshot, card_name, profile_name);
    audio_media_playback_snapshot_resume(playback_snapshot);
  }

  audio_media_playback_snapshot_free(playback_snapshot);
  audio_route_snapshot_free(snapshot);
  return error;
}
