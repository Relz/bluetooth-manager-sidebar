#include "audio/pactl-runner.h"

#include "core/timed-subprocess.h"

#define PACTL_LIST_TIMEOUT_SECONDS 3
#define PACTL_ROUTE_TIMEOUT_SECONDS 3
#define PACTL_SET_TIMEOUT_SECONDS 5

PactlError *
pactl_error_new(const char *reason, const char *detail)
{
  PactlError *error = g_new0(PactlError, 1);

  error->reason = g_strdup(reason ? reason : "pactl-failed");
  error->detail = g_strdup(detail);
  return error;
}

void
pactl_error_free(PactlError *error)
{
  if (!error)
    return;

  g_free(error->reason);
  g_free(error->detail);
  g_free(error);
}

static gboolean
pactl_spawn_sync_with_timeout(char **command,
                              guint timeout_seconds,
                              char **stdout_text,
                              char **stderr_text,
                              PactlError **out_error)
{
  TimedSubprocessResult result = { 0 };
  gboolean success;

  success = timed_subprocess_run(command,
                                 G_SUBPROCESS_FLAGS_STDOUT_PIPE | G_SUBPROCESS_FLAGS_STDERR_PIPE,
                                 timeout_seconds * 1000,
                                 &result);
  if (!success) {
    if (result.timed_out) {
      *out_error = pactl_error_new("pactl-timeout", NULL);
    } else if (result.error) {
      *out_error = pactl_error_new("pactl-failed", result.error->message);
    } else {
      if (result.stderr_text)
        g_strstrip(result.stderr_text);
      *out_error = pactl_error_new("pactl-failed", result.stderr_text);
    }
    timed_subprocess_result_clear(&result);
    return FALSE;
  }

  if (stdout_text)
    *stdout_text = g_steal_pointer(&result.stdout_text);
  if (stderr_text)
    *stderr_text = g_steal_pointer(&result.stderr_text);
  timed_subprocess_result_clear(&result);
  return TRUE;
}

gboolean
pactl_list_cards_json(char **stdout_text, PactlError **out_error)
{
  g_autofree char *pactl = g_find_program_in_path("pactl");
  g_autofree char *stderr_text = NULL;

  if (!pactl) {
    *out_error = pactl_error_new("pactl-unavailable", NULL);
    return FALSE;
  }

  char *argv[] = { pactl, "--format=json", "list", "cards", NULL };
  return pactl_spawn_sync_with_timeout(argv,
                                       PACTL_LIST_TIMEOUT_SECONDS,
                                       stdout_text,
                                       &stderr_text,
                                       out_error);
}

PactlError *
pactl_get_default_sink(char **sink_name)
{
  g_autofree char *pactl = NULL;
  g_autofree char *stdout_text = NULL;
  g_autofree char *stderr_text = NULL;
  PactlError *command_error = NULL;

  if (sink_name)
    *sink_name = NULL;

  pactl = g_find_program_in_path("pactl");
  if (!pactl)
    return pactl_error_new("pactl-unavailable", NULL);

  char *argv[] = { pactl, "get-default-sink", NULL };
  if (!pactl_spawn_sync_with_timeout(argv,
                                     PACTL_ROUTE_TIMEOUT_SECONDS,
                                     &stdout_text,
                                     &stderr_text,
                                     &command_error))
    return command_error;

  if (sink_name) {
    g_strstrip(stdout_text);
    *sink_name = g_steal_pointer(&stdout_text);
  }
  return NULL;
}

gboolean
pactl_list_sinks_json(char **stdout_text, PactlError **out_error)
{
  g_autofree char *pactl = g_find_program_in_path("pactl");
  g_autofree char *stderr_text = NULL;

  if (!pactl) {
    *out_error = pactl_error_new("pactl-unavailable", NULL);
    return FALSE;
  }

  char *argv[] = { pactl, "--format=json", "list", "sinks", NULL };
  return pactl_spawn_sync_with_timeout(argv,
                                       PACTL_LIST_TIMEOUT_SECONDS,
                                       stdout_text,
                                       &stderr_text,
                                       out_error);
}

gboolean
pactl_list_sink_inputs_json(char **stdout_text, PactlError **out_error)
{
  g_autofree char *pactl = g_find_program_in_path("pactl");
  g_autofree char *stderr_text = NULL;

  if (!pactl) {
    *out_error = pactl_error_new("pactl-unavailable", NULL);
    return FALSE;
  }

  char *argv[] = { pactl, "--format=json", "list", "sink-inputs", NULL };
  return pactl_spawn_sync_with_timeout(argv,
                                       PACTL_LIST_TIMEOUT_SECONDS,
                                       stdout_text,
                                       &stderr_text,
                                       out_error);
}

PactlError *
pactl_set_default_sink(const char *sink_name)
{
  g_autofree char *pactl = NULL;
  g_autofree char *stderr_text = NULL;
  PactlError *command_error = NULL;

  if (!sink_name || !*sink_name)
    return pactl_error_new("invalid-input", NULL);

  pactl = g_find_program_in_path("pactl");
  if (!pactl)
    return pactl_error_new("pactl-unavailable", NULL);

  char *argv[] = { pactl, "set-default-sink", (char *)sink_name, NULL };
  if (!pactl_spawn_sync_with_timeout(argv,
                                     PACTL_ROUTE_TIMEOUT_SECONDS,
                                     NULL,
                                     &stderr_text,
                                     &command_error))
    return command_error;

  return NULL;
}

PactlError *
pactl_move_sink_input(guint input_index, const char *sink_name)
{
  g_autofree char *pactl = NULL;
  g_autofree char *input = NULL;
  g_autofree char *stderr_text = NULL;
  PactlError *command_error = NULL;

  if (!sink_name || !*sink_name)
    return pactl_error_new("invalid-input", NULL);

  pactl = g_find_program_in_path("pactl");
  if (!pactl)
    return pactl_error_new("pactl-unavailable", NULL);

  input = g_strdup_printf("%u", input_index);
  char *argv[] = { pactl, "move-sink-input", input, (char *)sink_name, NULL };
  if (!pactl_spawn_sync_with_timeout(argv,
                                     PACTL_ROUTE_TIMEOUT_SECONDS,
                                     NULL,
                                     &stderr_text,
                                     &command_error))
    return command_error;

  return NULL;
}

PactlError *
pactl_set_card_profile(const char *card_name, const char *profile_name)
{
  g_autofree char *pactl = NULL;
  g_autofree char *stderr_text = NULL;
  PactlError *command_error = NULL;

  if (!card_name || !*card_name || !profile_name || !*profile_name)
    return pactl_error_new("invalid-input", NULL);

  pactl = g_find_program_in_path("pactl");
  if (!pactl)
    return pactl_error_new("pactl-unavailable", NULL);

  char *argv[] = { pactl, "set-card-profile", (char *)card_name, (char *)profile_name, NULL };
  if (!pactl_spawn_sync_with_timeout(argv,
                                     PACTL_SET_TIMEOUT_SECONDS,
                                     NULL,
                                     &stderr_text,
                                     &command_error))
    return command_error;

  return NULL;
}
