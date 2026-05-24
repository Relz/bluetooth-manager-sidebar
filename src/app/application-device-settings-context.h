#pragma once

#include "app/application.h"

#include "audio/audio-profiles.h"
#include "bluez/bluez-client.h"
#include "bluez/bluez-properties.h"

typedef struct {
  Application *self;
  const char *device_path;
  BluezProperties *device;
  AudioProfiles *audio_profiles;
  BluezClient *bluez;
} DeviceSettingsContext;

DeviceSettingsContext application_device_settings_context_new(Application *self);
