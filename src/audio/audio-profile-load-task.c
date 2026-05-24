#include "audio/audio-profile-load-task.h"

#include <gio/gio.h>

#include "audio/audio-profile-operations.h"
#include "audio/audio-profile-parser.h"
#include "audio/audio-profiles-internal.h"

typedef struct {
  char *device_path;
  char *address;
  AudioProfileLoadTaskFinished callback;
  gpointer user_data;
  GDestroyNotify destroy;
} AudioProfileLoadTask;

static void
audio_profile_load_task_free(AudioProfileLoadTask *request)
{
  if (!request)
    return;

  g_free(request->device_path);
  g_free(request->address);
  if (request->destroy && request->user_data)
    request->destroy(request->user_data);
  g_free(request);
}

static void
audio_profile_load_task_thread(GTask *task,
                               gpointer source_object,
                               gpointer task_data,
                               GCancellable *cancellable)
{
  AudioProfileLoadTask *request = task_data;
  g_autofree char *stdout_text = NULL;
  PactlError *command_error = NULL;
  AudioProfileInfo *info = NULL;

  (void)source_object;
  (void)cancellable;

  if (!request->address || !*request->address) {
    g_task_return_pointer(task,
                          audio_profile_info_new_error("missing-address", NULL),
                          (GDestroyNotify)audio_profile_info_free);
    return;
  }

  if (!audio_profile_operations_load_cards_json(&stdout_text, &command_error)) {
    info = audio_profile_info_new_error(command_error ? command_error->reason : "pactl-failed",
                                        command_error ? command_error->detail : NULL);
    pactl_error_free(command_error);
    g_task_return_pointer(task, info, (GDestroyNotify)audio_profile_info_free);
    return;
  }

  g_task_return_pointer(task,
                        audio_profile_parser_parse_cards(stdout_text, request->address),
                        (GDestroyNotify)audio_profile_info_free);
}

static void
audio_profile_load_task_finished(GObject *source_object, GAsyncResult *result, gpointer user_data)
{
  GTask *task = G_TASK(result);
  AudioProfileLoadTask *request = g_task_get_task_data(task);
  g_autoptr(GError) error = NULL;
  AudioProfileInfo *info = NULL;

  (void)source_object;
  (void)user_data;
  info = g_task_propagate_pointer(task, &error);
  if (!info)
    info = audio_profile_info_new_error("pactl-failed", error ? error->message : NULL);

  if (request->callback)
    request->callback(request->device_path, request->address, info, request->user_data);
  else
    audio_profile_info_free(info);
}

void
audio_profile_load_task_start(const char *device_path,
                              const char *address,
                              AudioProfileLoadTaskFinished callback,
                              gpointer user_data,
                              GDestroyNotify destroy)
{
  AudioProfileLoadTask *request = g_new0(AudioProfileLoadTask, 1);
  GTask *task;

  request->device_path = g_strdup(device_path ? device_path : "");
  request->address = g_strdup(address ? address : "");
  request->callback = callback;
  request->user_data = user_data;
  request->destroy = destroy;

  task = g_task_new(NULL, NULL, audio_profile_load_task_finished, NULL);
  g_task_set_task_data(task, request, (GDestroyNotify)audio_profile_load_task_free);
  g_task_run_in_thread(task, audio_profile_load_task_thread);
  g_object_unref(task);
}
