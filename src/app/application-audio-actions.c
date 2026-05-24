#include "app/application-audio-actions.h"

#include "app/application-audio-commands.h"

typedef struct {
  Application *self;
  char *card_name;
  char **profile_names;
} AudioProfileDropdown;

gpointer
application_actions_audio_profile_dropdown_new(Application *self,
                                               const char *card_name,
                                               char **profile_names)
{
  AudioProfileDropdown *dropdown = g_new0(AudioProfileDropdown, 1);

  dropdown->self = g_object_ref(self);
  dropdown->card_name = g_strdup(card_name ? card_name : "");
  dropdown->profile_names = profile_names;
  return dropdown;
}

void
application_actions_audio_profile_dropdown_free(gpointer data, GClosure *closure)
{
  AudioProfileDropdown *dropdown = data;

  (void)closure;
  if (!dropdown)
    return;

  g_clear_object(&dropdown->self);
  g_free(dropdown->card_name);
  g_strfreev(dropdown->profile_names);
  g_free(dropdown);
}

void
application_actions_on_audio_profile_selected(GObject *object, GParamSpec *param, gpointer user_data)
{
  GtkDropDown *dropdown = GTK_DROP_DOWN(object);
  AudioProfileDropdown *data = user_data;
  guint selected = gtk_drop_down_get_selected(dropdown);
  const char *profile_name;

  (void)param;
  if (!data->profile_names || selected == GTK_INVALID_LIST_POSITION)
    return;

  profile_name = data->profile_names[selected];
  if (!profile_name)
    return;

  application_commands_set_audio_profile(data->self, data->card_name, profile_name);
}
