#include "command/startup-command-router.h"

#include "app/app-metadata.h"
#include "command/command-ipc.h"
#include "core/target-output.h"

#define STARTUP_COMMAND_QUIT_WAIT_TIMEOUT_SECONDS 3.0
#define STARTUP_COMMAND_QUIT_WAIT_POLL_SECONDS 0.05

static StartupCommandRoute
startup_command_route_exit(int exit_status)
{
  return (StartupCommandRoute){
    .should_launch = FALSE,
    .start_hidden = FALSE,
    .exit_status = exit_status,
  };
}

static StartupCommandRoute
startup_command_route_launch(gboolean start_hidden)
{
  return (StartupCommandRoute){
    .should_launch = TRUE,
    .start_hidden = start_hidden,
    .exit_status = 0,
  };
}

StartupCommandRoute
startup_command_plan(SidebarCommand command,
                      const StartupCommandRouteInputs *inputs)
{
  if (sidebar_command_starts_hidden(command)) {
    if (inputs && inputs->server_acknowledged)
      return startup_command_route_exit(0);
    if (inputs && inputs->server_delivered)
      return startup_command_route_exit(1);
    return startup_command_route_launch(TRUE);
  }

  if (inputs && inputs->command_acknowledged)
    return startup_command_route_exit(0);
  if (inputs && inputs->command_delivered)
    return startup_command_route_exit(1);

  if (sidebar_command_is_ipc_only(command))
    return startup_command_route_exit(1);

  return startup_command_route_launch(FALSE);
}

static void
report_unacknowledged_command(SidebarCommand command)
{
  g_printerr("%s: --%s reached a listener but did not receive a %s acknowledgement; retry, or remove the socket only if that process is gone\n",
             COMMAND_NAME,
             sidebar_command_to_string(command),
             COMMAND_NAME);
}

static void
report_no_running_instance(SidebarCommand command)
{
  g_printerr("%s: no running instance reached for --%s\n",
             COMMAND_NAME,
             sidebar_command_to_string(command));
}

static int
report_quit_timeout(void)
{
  g_printerr("%s: timed out waiting for --quit to stop the running instance\n", COMMAND_NAME);
  return 1;
}

static int
finish_quit_command(void)
{
  gint64 deadline = g_get_monotonic_time() +
                    (gint64)(STARTUP_COMMAND_QUIT_WAIT_TIMEOUT_SECONDS * G_USEC_PER_SEC);

  while (TRUE) {
    g_autoptr(GError) error = NULL;
    CommandIpcStatus status = command_ipc_send(SIDEBAR_COMMAND_PING, NULL, &error);

    if (error) {
      g_printerr("%s: %s\n", COMMAND_NAME, error->message);
      return 1;
    }
    if (status == COMMAND_IPC_NOT_DELIVERED)
      return 0;
    if (g_get_monotonic_time() >= deadline)
      return report_quit_timeout();

    g_usleep((gulong)(STARTUP_COMMAND_QUIT_WAIT_POLL_SECONDS * G_USEC_PER_SEC));
  }
}

StartupCommandRoute
startup_command_route(SidebarCommand command)
{
  StartupCommandRouteInputs inputs = { 0 };
  g_autofree char *target_output_name = target_output_from_environment();
  g_autoptr(GError) error = NULL;
  CommandIpcStatus status;

  if (sidebar_command_starts_hidden(command)) {
    status = command_ipc_send(SIDEBAR_COMMAND_PING, NULL, &error);
    if (error) {
      g_printerr("%s: %s\n", COMMAND_NAME, error->message);
      return startup_command_route_exit(1);
    }
    inputs.server_acknowledged = status == COMMAND_IPC_ACKNOWLEDGED;
    inputs.server_delivered = status == COMMAND_IPC_DELIVERED;
    if (inputs.server_delivered)
      report_unacknowledged_command(command);
    return startup_command_plan(command, &inputs);
  }

  status = command_ipc_send(command, target_output_name, &error);
  if (error) {
    g_printerr("%s: %s\n", COMMAND_NAME, error->message);
    return startup_command_route_exit(1);
  }

  inputs.command_acknowledged = status == COMMAND_IPC_ACKNOWLEDGED;
  inputs.command_delivered = status == COMMAND_IPC_DELIVERED;

  if (inputs.command_acknowledged && command == SIDEBAR_COMMAND_QUIT)
    return startup_command_route_exit(finish_quit_command());

  if (inputs.command_delivered)
    report_unacknowledged_command(command);
  else if (status == COMMAND_IPC_NOT_DELIVERED && sidebar_command_is_ipc_only(command))
    report_no_running_instance(command);

  return startup_command_plan(command, &inputs);
}
