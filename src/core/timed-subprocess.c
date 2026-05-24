#include "core/timed-subprocess.h"

typedef struct {
  GMainLoop *loop;
  GCancellable *cancellable;
  GSubprocess *process;
  char *stdout_text;
  char *stderr_text;
  GError *error;
  gboolean timed_out;
  gboolean communicate_ok;
} TimedSubprocessRun;

static gboolean
timed_subprocess_on_timeout(gpointer user_data)
{
  TimedSubprocessRun *run = user_data;

  run->timed_out = TRUE;
  g_cancellable_cancel(run->cancellable);
  g_subprocess_force_exit(run->process);
  return G_SOURCE_REMOVE;
}

static void
timed_subprocess_on_finished(GObject *source_object,
                             GAsyncResult *result,
                             gpointer user_data)
{
  TimedSubprocessRun *run = user_data;

  run->communicate_ok = g_subprocess_communicate_utf8_finish(G_SUBPROCESS(source_object),
                                                             result,
                                                             &run->stdout_text,
                                                             &run->stderr_text,
                                                             &run->error);
  g_main_loop_quit(run->loop);
}

gboolean
timed_subprocess_run(char **argv,
                     GSubprocessFlags flags,
                     guint timeout_ms,
                     TimedSubprocessResult *result)
{
  g_autoptr(GError) spawn_error = NULL;
  g_autoptr(GSubprocess) process = NULL;
  g_autoptr(GCancellable) cancellable = NULL;
  g_autoptr(GMainContext) context = NULL;
  g_autoptr(GMainLoop) loop = NULL;
  GSource *timeout_source = NULL;
  TimedSubprocessRun run = { 0 };

  g_return_val_if_fail(argv != NULL, FALSE);
  g_return_val_if_fail(result != NULL, FALSE);

  *result = (TimedSubprocessResult){ 0 };
  process = g_subprocess_newv((const char *const *)argv, flags, &spawn_error);
  if (!process) {
    result->error = g_steal_pointer(&spawn_error);
    return FALSE;
  }

  context = g_main_context_new();
  loop = g_main_loop_new(context, FALSE);
  cancellable = g_cancellable_new();
  run.loop = loop;
  run.cancellable = cancellable;
  run.process = process;

  g_main_context_push_thread_default(context);
  g_subprocess_communicate_utf8_async(process,
                                      NULL,
                                      cancellable,
                                      timed_subprocess_on_finished,
                                      &run);
  timeout_source = g_timeout_source_new(timeout_ms);
  g_source_set_callback(timeout_source, timed_subprocess_on_timeout, &run, NULL);
  g_source_attach(timeout_source, context);
  g_main_loop_run(loop);
  g_source_destroy(timeout_source);
  g_source_unref(timeout_source);
  g_main_context_pop_thread_default(context);

  result->timed_out = run.timed_out;
  result->stdout_text = g_steal_pointer(&run.stdout_text);
  result->stderr_text = g_steal_pointer(&run.stderr_text);
  result->error = g_steal_pointer(&run.error);
  result->success = !result->timed_out && run.communicate_ok && g_subprocess_get_successful(process);

  return result->success;
}

void
timed_subprocess_result_clear(TimedSubprocessResult *result)
{
  if (!result)
    return;

  result->success = FALSE;
  result->timed_out = FALSE;
  g_clear_pointer(&result->stdout_text, g_free);
  g_clear_pointer(&result->stderr_text, g_free);
  g_clear_error(&result->error);
}
