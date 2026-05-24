#include "bluez/bluez-rfkill.h"

#include <gio/gio.h>

#include "core/timed-subprocess.h"

#define RFKILL_TIMEOUT_MS 5000
#define RFKILL_REASON_UNAVAILABLE "unavailable"
#define RFKILL_REASON_TIMEOUT "timeout"
#define RFKILL_REASON_FAILED "failed"

static void
rfkill_set_result(RfkillUnblockResult *result,
                  gboolean success,
                  const char *reason,
                  const char *detail)
{
  if (!result)
    return;

  result->success = success;
  result->reason = g_strdup(reason);
  result->detail = g_strdup(detail);
}

static char *
rfkill_dup_stripped_detail(const char *detail)
{
  char *stripped = NULL;

  if (!detail)
    return NULL;

  stripped = g_strdup(detail);
  g_strstrip(stripped);
  if (!*stripped)
    g_clear_pointer(&stripped, g_free);

  return stripped;
}

void
rfkill_unblock_result_clear(RfkillUnblockResult *result)
{
  if (!result)
    return;

  result->success = FALSE;
  g_clear_pointer(&result->reason, g_free);
  g_clear_pointer(&result->detail, g_free);
}

gboolean
rfkill_unblock_bluetooth(RfkillUnblockResult *out_result)
{
  TimedSubprocessResult command = { 0 };
  gboolean success;
  char *argv[] = { "rfkill", "unblock", "bluetooth", NULL };

  if (out_result)
    *out_result = (RfkillUnblockResult){ 0 };

  success = timed_subprocess_run(argv,
                                 G_SUBPROCESS_FLAGS_STDOUT_SILENCE | G_SUBPROCESS_FLAGS_STDERR_PIPE,
                                 RFKILL_TIMEOUT_MS,
                                 &command);
  if (success) {
    rfkill_set_result(out_result, TRUE, NULL, NULL);
  } else if (command.error && g_error_matches(command.error, G_SPAWN_ERROR, G_SPAWN_ERROR_NOENT)) {
    rfkill_set_result(out_result, FALSE, RFKILL_REASON_UNAVAILABLE, NULL);
  } else if (command.timed_out) {
    rfkill_set_result(out_result, FALSE, RFKILL_REASON_TIMEOUT, NULL);
  } else {
    g_autofree char *detail = rfkill_dup_stripped_detail(command.stderr_text);

    rfkill_set_result(out_result,
                      FALSE,
                      RFKILL_REASON_FAILED,
                      detail ? detail : (command.error ? command.error->message : NULL));
  }

  timed_subprocess_result_clear(&command);
  return success;
}
