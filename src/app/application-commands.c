#include "app/application-commands.h"

void
application_commands_handle_ipc(SidebarCommand command,
                                const char *target_output_name,
                                gpointer user_data)
{
  Application *self = APPLICATION(user_data);

  application_set_target_output_name(self, target_output_name);

  switch (command) {
    case SIDEBAR_COMMAND_SHOW:
      application_show_sidebar(self);
      break;
    case SIDEBAR_COMMAND_HIDE:
      application_hide_sidebar(self);
      break;
    case SIDEBAR_COMMAND_QUIT:
      g_application_quit(G_APPLICATION(self));
      break;
    case SIDEBAR_COMMAND_RELOAD_CSS:
      application_reload_user_css(self);
      break;
    case SIDEBAR_COMMAND_BACKGROUND:
    case SIDEBAR_COMMAND_PING:
      break;
    case SIDEBAR_COMMAND_TOGGLE:
    default:
      application_toggle_sidebar(self);
      break;
  }
}
