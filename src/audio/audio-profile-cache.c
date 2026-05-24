#include "audio/audio-profile-cache.h"

struct _AudioProfileCache {
  GHashTable *items;
};

AudioProfileCache *
audio_profile_cache_new(void)
{
  AudioProfileCache *cache = g_new0(AudioProfileCache, 1);

  cache->items = g_hash_table_new_full(g_str_hash,
                                       g_str_equal,
                                       g_free,
                                       (GDestroyNotify)audio_profile_info_free);
  return cache;
}

void
audio_profile_cache_free(AudioProfileCache *cache)
{
  if (!cache)
    return;

  g_hash_table_destroy(cache->items);
  g_free(cache);
}

AudioProfileInfo *
audio_profile_cache_lookup(AudioProfileCache *cache, const char *device_path)
{
  return cache && device_path ? g_hash_table_lookup(cache->items, device_path) : NULL;
}

AudioProfileInfo *
audio_profile_cache_dup_info(AudioProfileCache *cache, const char *device_path)
{
  AudioProfileInfo *info = audio_profile_cache_lookup(cache, device_path);

  return info ? audio_profile_info_copy(info) : NULL;
}

void
audio_profile_cache_replace(AudioProfileCache *cache,
                                const char *device_path,
                                AudioProfileInfo *info)
{
  if (cache && device_path)
    g_hash_table_replace(cache->items, g_strdup(device_path), info);
  else
    audio_profile_info_free(info);
}

void
audio_profile_cache_update_active_profile(AudioProfileCache *cache,
                                              const char *card_name,
                                              const char *profile_name)
{
  GHashTableIter iter;
  gpointer value = NULL;

  if (!cache)
    return;

  g_hash_table_iter_init(&iter, cache->items);
  while (g_hash_table_iter_next(&iter, NULL, &value)) {
    AudioProfileInfo *info = value;

    if (g_strcmp0(info->card_name, card_name) == 0) {
      g_free(info->active_profile);
      info->active_profile = g_strdup(profile_name);
    }
  }
}
