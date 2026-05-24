#include "app/application-device-audio-section-view.h"

#include <adwaita.h>

#include "app/application-audio-profile-model.h"
#include "app/application-page-status.h"
#include "app/application-page-utils.h"
#include "core/display.h"

static const char *
application_device_audio_section_view_empty_subtitle(AudioProfileInfo *info)
{
  if (info && info->loading)
    return "Loading audio profiles...";
  if (info && application_page_string_has_text(info->error))
    return info->error;

  return "No audio profiles available for this device.";
}

GtkWidget *
application_device_audio_section_view_empty_row(AudioProfileInfo *info)
{
  return application_page_action_row("Audio profile",
                                     application_device_audio_section_view_empty_subtitle(info));
}

GtkWidget *
application_device_audio_section_view_choices_row(AudioProfileInfo *info,
                                                  GtkWidget **out_dropdown)
{
  GtkWidget *row = adw_preferences_row_new();
  GtkWidget *profile_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  GtkWidget *profile_label = gtk_label_new("Audio profile");
  g_auto(GStrv) labels = application_audio_profile_model_dup_labels(info);
  GtkWidget *dropdown = gtk_drop_down_new_from_strings((const char *const *)labels);

  adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), "Audio profile");
  gtk_widget_set_margin_top(profile_box, 8);
  gtk_widget_set_margin_bottom(profile_box, 8);
  gtk_widget_set_margin_start(profile_box, 12);
  gtk_widget_set_margin_end(profile_box, 12);
  gtk_label_set_xalign(GTK_LABEL(profile_label), 0.0f);
  gtk_widget_add_css_class(profile_label, "caption-heading");
  gtk_box_append(GTK_BOX(profile_box), profile_label);

  gtk_widget_set_hexpand(dropdown, TRUE);
  gtk_drop_down_set_selected(GTK_DROP_DOWN(dropdown), application_audio_profile_model_selected_index(info));
  gtk_box_append(GTK_BOX(profile_box), dropdown);
  gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), profile_box);
  gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(row), FALSE);

  if (application_page_string_has_text(info->error)) {
    g_autofree char *safe_error = display_dup_safe_text(info->error, "Unknown error");
    GtkWidget *error_label = gtk_label_new(safe_error ? safe_error : "Unknown error");

    gtk_label_set_xalign(GTK_LABEL(error_label), 0.0f);
    gtk_label_set_wrap(GTK_LABEL(error_label), TRUE);
    gtk_widget_add_css_class(error_label, "dim-label");
    gtk_box_append(GTK_BOX(profile_box), error_label);
  }

  if (out_dropdown)
    *out_dropdown = dropdown;
  return row;
}
