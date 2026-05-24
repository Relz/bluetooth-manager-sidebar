#include "app/application-device-actions.h"

#include "app/application-action-data.h"
#include "app/application-bluetooth-commands.h"
#include "app/application-device-removal-flow.h"
#include "app/application-pages.h"

void
application_actions_on_device_activate(GtkWidget *widget, gpointer user_data)
{
  Application *self = APPLICATION(user_data);
  const char *device_path = application_action_widget_device_path(widget);

  application_commands_activate_device(self, device_path);
}

void
application_actions_on_trust_device_clicked(GtkButton *button, gpointer user_data)
{
  Application *self = APPLICATION(user_data);
  const char *device_path = application_action_widget_device_path(GTK_WIDGET(button));

  application_commands_trust_device(self, device_path);
}

void
application_actions_on_device_settings_clicked(GtkButton *button, gpointer user_data)
{
  Application *self = APPLICATION(user_data);
  const char *device_path = application_action_widget_device_path(GTK_WIDGET(button));

  application_pages_show_device_settings(self, device_path);
}

static void
application_actions_commit_alias_entry(Application *self, GtkEntry *entry)
{
  const char *device_path = application_action_widget_device_path(GTK_WIDGET(entry));

  application_commands_set_device_alias(self,
                                        device_path,
                                        gtk_editable_get_text(GTK_EDITABLE(entry)));
}

void
application_actions_on_alias_entry_activated(GtkEntry *entry, gpointer user_data)
{
  application_actions_commit_alias_entry(APPLICATION(user_data), entry);
}

void
application_actions_on_alias_entry_focus_changed(GObject *object,
                                                 GParamSpec *param,
                                                 gpointer user_data)
{
  (void)param;
  if (!gtk_widget_has_focus(GTK_WIDGET(object)))
    application_actions_commit_alias_entry(APPLICATION(user_data), GTK_ENTRY(object));
}

void
application_actions_on_device_trusted_changed(GObject *object, GParamSpec *param, gpointer user_data)
{
  Application *self = APPLICATION(user_data);
  const char *device_path = application_action_widget_device_path(GTK_WIDGET(object));

  (void)param;
  application_commands_set_device_trusted(self,
                                          device_path,
                                          gtk_switch_get_active(GTK_SWITCH(object)));
}

void
application_actions_on_device_blocked_changed(GObject *object, GParamSpec *param, gpointer user_data)
{
  Application *self = APPLICATION(user_data);
  const char *device_path = application_action_widget_device_path(GTK_WIDGET(object));

  (void)param;
  application_commands_set_device_blocked(self,
                                          device_path,
                                          gtk_switch_get_active(GTK_SWITCH(object)));
}

void
application_actions_on_remove_device_clicked(GtkButton *button, gpointer user_data)
{
  Application *self = APPLICATION(user_data);
  const char *device_path = application_action_widget_device_path(GTK_WIDGET(button));

  application_device_removal_flow_present(self, device_path);
}
