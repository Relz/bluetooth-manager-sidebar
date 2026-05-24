#include "audio/audio-profile-setter.h"

#include "audio/audio-profile-messages.h"
#include "audio/audio-profile-operations.h"
#include "audio/audio-profile-set-task.h"

typedef struct {
  AudioProfileSetStateChanged state_changed;
  gpointer state_user_data;
  GDestroyNotify state_destroy;
  AudioProfileSetFinished callback;
  gpointer callback_user_data;
  GDestroyNotify callback_destroy;
} AudioProfileSetRequest;

static void
audio_profile_set_request_free(AudioProfileSetRequest *request)
{
  if (!request)
    return;

  if (request->state_destroy && request->state_user_data)
    request->state_destroy(request->state_user_data);
  if (request->callback_destroy && request->callback_user_data)
    request->callback_destroy(request->callback_user_data);
  g_free(request);
}

char *
audio_profile_setter_set_profile(const char *card_name,
                                 const char *profile_name,
                                 gboolean *out_success)
{
  PactlError *error = audio_profile_operations_set_profile(card_name, profile_name);

  if (out_success)
    *out_success = error == NULL;
  if (error) {
    char *message = audio_profile_change_message(error->reason, error->detail);

    pactl_error_free(error);
    return message;
  }

  return audio_profile_change_message(NULL, NULL);
}

static void
audio_profile_setter_set_finished(const char *card_name,
                                  const char *profile_name,
                                  PactlError *error,
                                  gpointer user_data)
{
  AudioProfileSetRequest *request = user_data;
  g_autofree char *message = NULL;
  gboolean success = error == NULL;
  gboolean owner_active = TRUE;

  if (error) {
    message = audio_profile_change_message(error->reason, error->detail);
    pactl_error_free(error);
  } else {
    message = audio_profile_change_message(NULL, NULL);
  }

  if (request->state_changed)
    owner_active = request->state_changed(card_name, profile_name, success, request->state_user_data);
  if (owner_active && request->callback)
    request->callback(message, request->callback_user_data);
}

void
audio_profile_setter_set_profile_async(const char *card_name,
                                       const char *profile_name,
                                       AudioProfileSetStateChanged state_changed,
                                       gpointer state_user_data,
                                       GDestroyNotify state_destroy,
                                       AudioProfileSetFinished callback,
                                       gpointer callback_user_data,
                                       GDestroyNotify callback_destroy)
{
  AudioProfileSetRequest *request = g_new0(AudioProfileSetRequest, 1);

  request->state_changed = state_changed;
  request->state_user_data = state_user_data;
  request->state_destroy = state_destroy;
  request->callback = callback;
  request->callback_user_data = callback_user_data;
  request->callback_destroy = callback_destroy;

  audio_profile_set_task_start(card_name,
                               profile_name,
                               audio_profile_setter_set_finished,
                               request,
                               (GDestroyNotify)audio_profile_set_request_free);
}
