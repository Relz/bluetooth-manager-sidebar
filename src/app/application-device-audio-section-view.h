#pragma once

#include <gtk/gtk.h>

#include "audio/audio-profiles.h"

GtkWidget *application_device_audio_section_view_empty_row(AudioProfileInfo *info);
GtkWidget *application_device_audio_section_view_choices_row(AudioProfileInfo *info,
                                                            GtkWidget **out_dropdown);
