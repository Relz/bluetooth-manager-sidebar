#include "app/application-page-utils.h"

#include "core/display.h"

GtkWidget *
application_page_action_row(const char *title, const char *subtitle)
{
  GtkWidget *row = adw_action_row_new();
  g_autofree char *safe_title = display_dup_safe_text(title, "");
  g_autofree char *safe_subtitle = display_dup_safe_text(subtitle, NULL);

  adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), safe_title ? safe_title : "");
  if (safe_subtitle)
    adw_action_row_set_subtitle(ADW_ACTION_ROW(row), safe_subtitle);
  adw_action_row_set_subtitle_lines(ADW_ACTION_ROW(row), 3);
  return row;
}

GtkWidget *
application_page_labeled_entry_row(const char *title,
                                   const char *text,
                                   const char *placeholder,
                                   GtkWidget **out_entry)
{
  GtkWidget *row = adw_preferences_row_new();
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  g_autofree char *safe_title = display_dup_safe_text(title, "");
  GtkWidget *label = gtk_label_new(safe_title ? safe_title : "");
  GtkWidget *entry = gtk_entry_new();

  adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), safe_title ? safe_title : "");
  gtk_widget_set_margin_top(box, 8);
  gtk_widget_set_margin_bottom(box, 8);
  gtk_widget_set_margin_start(box, 12);
  gtk_widget_set_margin_end(box, 12);

  gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
  gtk_widget_add_css_class(label, "caption-heading");
  gtk_box_append(GTK_BOX(box), label);

  gtk_widget_set_hexpand(entry, TRUE);
  gtk_editable_set_width_chars(GTK_EDITABLE(entry), 22);
  if (placeholder)
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), placeholder);
  gtk_editable_set_text(GTK_EDITABLE(entry), text ? text : "");
  gtk_box_append(GTK_BOX(box), entry);

  gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), box);
  gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(row), FALSE);
  if (out_entry)
    *out_entry = entry;
  return row;
}

GtkWidget *
application_page_selectable_property_row(const char *title, const char *value)
{
  GtkWidget *row = application_page_action_row(title, NULL);
  g_autofree char *safe_value = display_dup_safe_text(value, "Unknown");
  GtkWidget *value_label = gtk_label_new(safe_value ? safe_value : "Unknown");

  gtk_list_box_row_set_selectable(GTK_LIST_BOX_ROW(row), TRUE);
  gtk_label_set_xalign(GTK_LABEL(value_label), 1.0f);
  gtk_label_set_wrap(GTK_LABEL(value_label), TRUE);
  gtk_label_set_selectable(GTK_LABEL(value_label), TRUE);
  gtk_widget_set_hexpand(value_label, TRUE);
  gtk_widget_add_css_class(value_label, "dim-label");
  adw_action_row_add_suffix(ADW_ACTION_ROW(row), value_label);
  return row;
}

GtkWidget *
application_page_switch_row(const char *title,
                            gboolean active,
                            gboolean sensitive,
                            GtkWidget **out_switch)
{
  GtkWidget *row = application_page_action_row(title, NULL);
  GtkWidget *toggle = gtk_switch_new();

  gtk_widget_set_valign(toggle, GTK_ALIGN_CENTER);
  gtk_switch_set_active(GTK_SWITCH(toggle), active);
  gtk_widget_set_sensitive(toggle, sensitive);
  adw_action_row_add_suffix(ADW_ACTION_ROW(row), toggle);
  adw_action_row_set_activatable_widget(ADW_ACTION_ROW(row), toggle);
  if (out_switch)
    *out_switch = toggle;
  return row;
}

GtkWidget *
application_page_button_row(const char *title,
                            const char *subtitle,
                            const char *button_label,
                            gboolean destructive,
                            GtkWidget **out_button)
{
  GtkWidget *row = application_page_action_row(title, subtitle);
  GtkWidget *button = gtk_button_new_with_label(button_label);

  gtk_widget_set_valign(button, GTK_ALIGN_CENTER);
  if (destructive)
    gtk_widget_add_css_class(button, "destructive-action");
  adw_action_row_add_suffix(ADW_ACTION_ROW(row), button);
  adw_action_row_set_activatable_widget(ADW_ACTION_ROW(row), button);
  if (out_button)
    *out_button = button;
  return row;
}
