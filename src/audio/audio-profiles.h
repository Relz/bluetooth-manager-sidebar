#pragma once

#include <glib.h>

typedef struct _AudioProfiles AudioProfiles;

typedef struct {
  char *name;
  char *label;
} AudioProfileOption;

typedef struct {
  gboolean loading;
  char *address;
  char *card_name;
  char *active_profile;
  char *error;
  GPtrArray *profiles;
} AudioProfileInfo;

typedef void (*AudioProfilesChanged)(gpointer user_data);
typedef void (*AudioProfileSetFinished)(const char *message, gpointer user_data);

AudioProfiles *audio_profiles_new(AudioProfilesChanged changed,
                                          gpointer user_data,
                                          GDestroyNotify destroy);
void audio_profiles_free(AudioProfiles *profiles);

AudioProfileInfo *audio_profile_info_new(void);
AudioProfileInfo *audio_profile_info_copy(const AudioProfileInfo *info);
void audio_profile_info_free(AudioProfileInfo *info);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(AudioProfileInfo, audio_profile_info_free)

AudioProfileInfo *audio_profiles_dup_info(AudioProfiles *profiles,
                                                 const char *device_path);
void audio_profiles_load(AudioProfiles *profiles,
                             const char *device_path,
                             const char *address);
char *audio_profiles_set_profile(AudioProfiles *profiles,
                                     const char *card_name,
                                     const char *profile_name);
void audio_profiles_set_profile_async(AudioProfiles *profiles,
                                          const char *card_name,
                                          const char *profile_name,
                                          AudioProfileSetFinished callback,
                                          gpointer user_data,
                                          GDestroyNotify destroy);
