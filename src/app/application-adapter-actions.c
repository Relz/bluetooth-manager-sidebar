#include "app/application-adapter-actions.h"

#include "app/application-bluetooth-commands.h"

void
application_actions_on_adapter_power_changed(GObject *object, GParamSpec *param, gpointer user_data)
{
  Application *self = APPLICATION(user_data);

  (void)param;
  application_commands_set_adapter_power(self, gtk_switch_get_active(GTK_SWITCH(object)));
}

void
application_actions_on_discoverable_changed(GObject *object, GParamSpec *param, gpointer user_data)
{
  Application *self = APPLICATION(user_data);

  (void)param;
  application_commands_set_adapter_discoverable(self, gtk_switch_get_active(GTK_SWITCH(object)));
}

void
application_actions_on_scan_clicked(GtkButton *button, gpointer user_data)
{
  Application *self = APPLICATION(user_data);

  (void)button;
  application_commands_toggle_discovery(self);
}
