#pragma once

#include <glib.h>

typedef struct {
  char *reason;
  char *detail;
} PactlError;

PactlError *pactl_error_new(const char *reason, const char *detail);
void pactl_error_free(PactlError *error);

gboolean pactl_list_cards_json(char **stdout_text, PactlError **out_error);
PactlError *pactl_get_default_sink(char **sink_name);
gboolean pactl_list_sinks_json(char **stdout_text, PactlError **out_error);
gboolean pactl_list_sink_inputs_json(char **stdout_text, PactlError **out_error);
PactlError *pactl_set_default_sink(const char *sink_name);
PactlError *pactl_move_sink_input(guint input_index, const char *sink_name);
PactlError *pactl_set_card_profile(const char *card_name, const char *profile_name);
