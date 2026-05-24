#include "audio/audio-profile-set-task.h"

#include <gio/gio.h>

#include "audio/audio-profile-operations.h"

typedef struct {
  char *card_name;
  char *profile_name;
  AudioProfileSetTaskFinished callback;
  gpointer user_data;
  GDestroyNotify destroy;
} AudioProfileSetTask;

static void
audio_profile_set_task_free(AudioProfileSetTask *request)
{
  if (!request)
    return;

  g_free(request->card_name);
  g_free(request->profile_name);
  if (request->destroy && request->user_data)
    request->destroy(request->user_data);
  g_free(request);
}

static void
audio_profile_set_task_thread(GTask *task,
                              gpointer source_object,
                              gpointer task_data,
                              GCancellable *cancellable)
{
  AudioProfileSetTask *request = task_data;

  (void)source_object;
  (void)cancellable;
  g_task_return_pointer(task,
                        audio_profile_operations_set_profile(request->card_name, request->profile_name),
                        (GDestroyNotify)pactl_error_free);
}

static void
audio_profile_set_task_finished(GObject *source_object, GAsyncResult *result, gpointer user_data)
{
  GTask *task = G_TASK(result);
  AudioProfileSetTask *request = g_task_get_task_data(task);
  PactlError *error = g_task_propagate_pointer(task, NULL);

  (void)source_object;
  (void)user_data;
  if (request->callback)
    request->callback(request->card_name, request->profile_name, error, request->user_data);
  else
    pactl_error_free(error);
}

void
audio_profile_set_task_start(const char *card_name,
                             const char *profile_name,
                             AudioProfileSetTaskFinished callback,
                             gpointer user_data,
                             GDestroyNotify destroy)
{
  AudioProfileSetTask *request = g_new0(AudioProfileSetTask, 1);
  GTask *task;

  request->card_name = g_strdup(card_name ? card_name : "");
  request->profile_name = g_strdup(profile_name ? profile_name : "");
  request->callback = callback;
  request->user_data = user_data;
  request->destroy = destroy;

  task = g_task_new(NULL, NULL, audio_profile_set_task_finished, NULL);
  g_task_set_task_data(task, request, (GDestroyNotify)audio_profile_set_task_free);
  g_task_run_in_thread(task, audio_profile_set_task_thread);
  g_object_unref(task);
}
