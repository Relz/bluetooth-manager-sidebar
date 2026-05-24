#include "app/application-overview-view.h"

#include "app/application-device-view.h"
#include "app/application-overview-presentation.h"
#include "app/application-page-status.h"
#include "app/application-page-utils.h"

GtkWidget *
application_overview_view_device_row(const OverviewDeviceRowModel *model)
{
  GtkWidget *row = application_page_action_row(model->title, model->subtitle);

  adw_action_row_add_prefix(ADW_ACTION_ROW(row),
                            gtk_image_new_from_icon_name(application_device_view_icon_name(model->device)));
  if (model->blocked)
    gtk_widget_add_css_class(row, "blocked-device-row");
  else if (model->connected)
    gtk_widget_add_css_class(row, "active-device-row");
  else if (model->connecting)
    gtk_widget_add_css_class(row, "connecting-device-row");
  else if (model->paired)
    gtk_widget_add_css_class(row, "inactive-device-row");

  return row;
}

GtkWidget *
application_overview_view_discoverable_row(gboolean discoverable,
                                           gboolean powered,
                                           GtkWidget **out_toggle)
{
  GtkWidget *row = application_page_action_row(
    application_overview_presentation_discoverable_title(),
    application_overview_presentation_discoverable_subtitle());
  GtkWidget *toggle = gtk_switch_new();

  gtk_widget_set_valign(toggle, GTK_ALIGN_CENTER);
  gtk_widget_set_tooltip_text(toggle, application_overview_presentation_discoverable_tooltip());
  gtk_switch_set_active(GTK_SWITCH(toggle), discoverable);
  gtk_widget_set_sensitive(toggle, powered);
  adw_action_row_add_suffix(ADW_ACTION_ROW(row), toggle);
  adw_action_row_set_activatable_widget(ADW_ACTION_ROW(row), toggle);
  if (out_toggle)
    *out_toggle = toggle;
  return row;
}

GtkWidget *
application_overview_view_scan_button(gboolean discovering)
{
  GtkWidget *button = gtk_button_new_with_label(
    application_overview_presentation_scan_button_label(discovering));

  gtk_widget_set_tooltip_text(button, application_overview_presentation_scan_button_tooltip());
  return button;
}

GtkWidget *
application_overview_view_status_group(void)
{
  GtkWidget *group = adw_preferences_group_new();

  adw_preferences_group_set_title(ADW_PREFERENCES_GROUP(group),
                                  application_overview_presentation_status_group_title());
  return group;
}

GtkWidget *
application_overview_view_device_group(const char *title)
{
  GtkWidget *group = adw_preferences_group_new();

  adw_preferences_group_set_title(ADW_PREFERENCES_GROUP(group), title);
  gtk_widget_add_css_class(group, "device-group");
  return group;
}

void
application_overview_view_add_status_rows(const OverviewStatusModel *status,
                                          AdwPreferencesGroup *group)
{
  if (application_page_string_has_text(status->pairing_message))
    adw_preferences_group_add(group, application_page_action_row("Pairing agent", status->pairing_message));
  if (application_page_string_has_text(status->obex_message))
    adw_preferences_group_add(group, application_page_action_row("File receiving", status->obex_message));
}

void
application_overview_view_add_blocked_power_row(AdwPreferencesGroup *status_group,
                                               const OverviewModel *model)
{
  if (!application_overview_model_has_blocked_power_status(model))
    return;

  adw_preferences_group_add(
    status_group,
    application_page_action_row(
      application_overview_presentation_blocked_power_title(),
      application_overview_presentation_blocked_power_subtitle()));
}

void
application_overview_view_add_available_empty_row(AdwPreferencesGroup *group,
                                                 gboolean powered,
                                                 gboolean discovering)
{
  adw_preferences_group_add(
    group,
    application_page_action_row(application_overview_presentation_available_empty_title(powered, discovering),
                                application_overview_presentation_available_empty_subtitle(powered, discovering)));
}

void
application_overview_view_add_no_adapter_row(AdwPreferencesGroup *group, const char *message)
{
  adw_preferences_group_add(ADW_PREFERENCES_GROUP(group),
                            application_page_action_row(
                              application_overview_presentation_no_adapter_title(),
                              message));
}
