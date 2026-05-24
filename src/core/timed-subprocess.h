#pragma once

#include <gio/gio.h>

typedef struct {
  gboolean success;
  gboolean timed_out;
  char *stdout_text;
  char *stderr_text;
  GError *error;
} TimedSubprocessResult;

gboolean timed_subprocess_run(char **argv,
                             GSubprocessFlags flags,
                             guint timeout_ms,
                             TimedSubprocessResult *result);
void timed_subprocess_result_clear(TimedSubprocessResult *result);
