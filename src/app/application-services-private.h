#pragma once

#include "app/application-services.h"

#include "app/application-audio-runtime.h"
#include "app/application-bluez-runtime.h"
#include "app/application-command-runtime.h"
#include "app/application-obex-runtime.h"
#include "app/application-settings-runtime.h"

struct _ApplicationServices {
  ApplicationCommandRuntime *command;
  ApplicationBluezRuntime *bluez;
  ApplicationAudioRuntime *audio;
  ApplicationObexRuntime *obex;
  ApplicationSettingsRuntime *settings;
};
