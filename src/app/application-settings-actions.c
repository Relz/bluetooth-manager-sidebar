#include "app/application-settings-actions.h"

#include "app/application-settings-commands.h"

void
application_actions_on_auto_accept_changed(GObject *object, GParamSpec *param, gpointer user_data)
{
  Application *self = APPLICATION(user_data);

  (void)param;
  application_commands_set_auto_accept_trusted_files(self, gtk_switch_get_active(GTK_SWITCH(object)));
}

static void
application_actions_commit_received_location_entry(Application *self, GtkEntry *entry)
{
  application_commands_set_received_files_location(self,
                                                   gtk_editable_get_text(GTK_EDITABLE(entry)));
}

void
application_actions_on_received_location_activated(GtkEntry *entry, gpointer user_data)
{
  application_actions_commit_received_location_entry(APPLICATION(user_data), entry);
}

void
application_actions_on_received_location_focus_changed(GObject *object,
                                                       GParamSpec *param,
                                                       gpointer user_data)
{
  (void)param;
  if (!gtk_widget_has_focus(GTK_WIDGET(object)))
    application_actions_commit_received_location_entry(APPLICATION(user_data), GTK_ENTRY(object));
}
