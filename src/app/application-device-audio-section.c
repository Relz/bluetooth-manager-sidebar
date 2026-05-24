#include "app/application-device-settings-sections.h"

#include "app/application-device-audio-section-actions.h"
#include "app/application-device-audio-section-model.h"
#include "app/application-device-audio-section-view.h"

static GtkWidget *
application_audio_profile_row(Application *self,
                               const char *device_path,
                               AudioProfiles *audio_profiles)
{
  DeviceAudioSectionModel model = application_device_audio_section_model_new(audio_profiles, device_path);
  AudioProfileInfo *info = model.info;
  GtkWidget *row = NULL;
  GtkWidget *dropdown = NULL;

  if (!application_device_audio_section_model_has_choices(&model)) {
    row = application_device_audio_section_view_empty_row(info);
    application_device_audio_section_model_clear(&model);
    return row;
  }

  row = application_device_audio_section_view_choices_row(info, &dropdown);
  application_device_audio_section_actions_bind_dropdown(self, info, dropdown);
  application_device_audio_section_model_clear(&model);
  return row;
}

void
application_device_settings_sections_add_audio(const DeviceSettingsContext *context,
                                               AdwPreferencesGroup *group)
{
  adw_preferences_group_add(group,
                            application_audio_profile_row(context->self,
                                                          context->device_path,
                                                          context->audio_profiles));
}
