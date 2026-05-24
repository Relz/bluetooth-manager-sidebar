#include "app/application-overview-actions.h"

#include "app/application-action-data.h"
#include "app/application-adapter-actions.h"
#include "app/application-device-actions.h"
#include "app/application-overview-presentation.h"

static GtkWidget *
application_overview_device_action_button(const char *icon_name,
                                          const char *tooltip,
                                          const char *device_path,
                                          GCallback callback,
                                          Application *self)
{
  GtkWidget *button = gtk_button_new_from_icon_name(icon_name);

  gtk_widget_add_css_class(button, "flat");
  gtk_widget_set_tooltip_text(button, tooltip);
  gtk_widget_set_valign(button, GTK_ALIGN_CENTER);
  application_action_set_widget_device_path(button, device_path);
  g_signal_connect(button, "clicked", callback, self);
  return button;
}

static GCallback
application_overview_device_action_callback(OverviewDeviceAction action)
{
  switch (action) {
    case APPLICATION_OVERVIEW_DEVICE_ACTION_TRUST:
      return G_CALLBACK(application_actions_on_trust_device_clicked);
    case APPLICATION_OVERVIEW_DEVICE_ACTION_SETTINGS:
      return G_CALLBACK(application_actions_on_device_settings_clicked);
  }

  return NULL;
}

void
application_overview_actions_bind_device_row(Application *self,
                                             GtkWidget *row,
                                             const char *device_path,
                                             const OverviewDeviceRowModel *model)
{
  OverviewDeviceActionModel actions[2];
  guint action_count;

  application_action_set_widget_device_path(row, device_path);
  gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(row), TRUE);
  g_signal_connect(row, "activated", G_CALLBACK(application_actions_on_device_activate), self);

  action_count = application_overview_presentation_device_actions(model,
                                                                 actions,
                                                                 G_N_ELEMENTS(actions));
  for (guint i = 0; i < action_count; i++)
    adw_action_row_add_suffix(ADW_ACTION_ROW(row),
                              application_overview_device_action_button(
                                actions[i].icon_name,
                                actions[i].tooltip,
                                device_path,
                                application_overview_device_action_callback(actions[i].action),
                                self));
}

void
application_overview_actions_bind_discoverable_toggle(Application *self, GtkWidget *toggle)
{
  g_signal_connect(toggle,
                   "notify::active",
                   G_CALLBACK(application_actions_on_discoverable_changed),
                   self);
}

void
application_overview_actions_bind_scan_button(Application *self, GtkWidget *button)
{
  g_signal_connect(button, "clicked", G_CALLBACK(application_actions_on_scan_clicked), self);
}
