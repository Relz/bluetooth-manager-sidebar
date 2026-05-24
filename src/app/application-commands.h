#pragma once

#include "app/application.h"
#include "command/sidebar-command.h"

void application_commands_handle_ipc(SidebarCommand command,
                                     const char *target_output_name,
                                     gpointer user_data);
