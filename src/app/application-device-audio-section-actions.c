#include "app/application-device-audio-section-actions.h"

#include "app/application-audio-actions.h"
#include "app/application-audio-profile-model.h"

void
application_device_audio_section_actions_bind_dropdown(Application *self,
                                                       AudioProfileInfo *info,
                                                       GtkWidget *dropdown)
{
  gpointer data = application_actions_audio_profile_dropdown_new(
    self,
    info->card_name ? info->card_name : "",
    application_audio_profile_model_dup_profile_names(info));

  g_signal_connect_data(dropdown,
                        "notify::selected",
                        G_CALLBACK(application_actions_on_audio_profile_selected),
                        data,
                        application_actions_audio_profile_dropdown_free,
                        0);
}
