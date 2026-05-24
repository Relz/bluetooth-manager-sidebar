#pragma once

#include <glib.h>

#include "command/sidebar-command.h"

typedef struct {
  gboolean should_launch;
  gboolean start_hidden;
  int exit_status;
} StartupCommandRoute;

typedef struct {
  gboolean server_acknowledged;
  gboolean server_delivered;
  gboolean command_acknowledged;
  gboolean command_delivered;
} StartupCommandRouteInputs;

StartupCommandRoute startup_command_plan(SidebarCommand command,
                                            const StartupCommandRouteInputs *inputs);
StartupCommandRoute startup_command_route(SidebarCommand command);
