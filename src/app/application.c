#include "app/application-private.h"

#include "app/application-commands.h"
#include "app/application-obex-adapter.h"
#include "app/application-obex-notification-actions.h"
#include "app/application-services.h"
#include "app/application-styles.h"
#include "app/app-metadata.h"
#include "core/target-output.h"
#include "sidebar/sidebar-window.h"

G_DEFINE_TYPE(Application, application, ADW_TYPE_APPLICATION)

static int application_command_line(GApplication *application, GApplicationCommandLine *command_line);
static int application_handle_command_line(Application *self,
                                           SidebarCommand command,
                                           const char *target_output_name,
                                           gboolean is_remote);
static char *application_target_output_from_command_line(GApplicationCommandLine *command_line);

static void
application_startup(GApplication *application)
{
  Application *self = APPLICATION(application);
  g_autoptr(GError) error = NULL;

  G_APPLICATION_CLASS(application_parent_class)->startup(application);
  g_application_hold(application);

  if (!application_styles_install(self)) {
    self->runtime_failure = TRUE;
    g_application_release(application);
    g_application_quit(application);
    return;
  }
  application_obex_notification_actions_install(self);
  if (!application_services_start(self, &error)) {
    self->runtime_failure = TRUE;
    g_printerr("%s: Unable to start command socket: %s\n",
               COMMAND_NAME,
               error ? error->message : "unknown error");
    g_clear_error(&error);
    g_application_release(application);
    g_application_quit(application);
  }
}

static void
application_activate(GApplication *application)
{
  Application *self = APPLICATION(application);

  if (self->runtime_failure)
    return;

  if (self->start_hidden) {
    self->start_hidden = FALSE;
    return;
  }

  application_show_sidebar(self);
}

static void
application_shutdown(GApplication *application)
{
  Application *self = APPLICATION(application);

  application_styles_clear_user_css_provider();
  g_clear_handle_id(&self->refresh_source, g_source_remove);
  g_clear_handle_id(&self->signal_refresh_source, g_source_remove);
  application_services_stop(self);
  g_clear_pointer(&self->target_output_name, g_free);
  g_clear_pointer(&self->page_controller, application_page_controller_free);
  G_APPLICATION_CLASS(application_parent_class)->shutdown(application);
}

static void
application_class_init(ApplicationClass *klass)
{
  GApplicationClass *application_class = G_APPLICATION_CLASS(klass);

  application_class->startup = application_startup;
  application_class->activate = application_activate;
  application_class->command_line = application_command_line;
  application_class->shutdown = application_shutdown;
}

static void
application_init(Application *self)
{
  self->page_controller = application_page_controller_new();
}

gboolean
application_command_from_arguments(char **argv,
                                   int argc,
                                   SidebarCommand *out_command,
                                   GError **error)
{
  SidebarCommand command = SIDEBAR_COMMAND_TOGGLE;
  gboolean command_seen = FALSE;

  g_return_val_if_fail(out_command != NULL, FALSE);

  for (int i = 1; i < argc; i++) {
    const char *arg = argv[i];

    if (g_str_has_prefix(arg, "--"))
      arg += 2;

    if (command_seen) {
      g_set_error(error,
                  G_OPTION_ERROR,
                  G_OPTION_ERROR_FAILED,
                  "argument %s: not allowed with another command option",
                  argv[i]);
      return FALSE;
    }
    if (!sidebar_command_parse_text(arg, &command) || command == SIDEBAR_COMMAND_PING) {
      g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_UNKNOWN_OPTION, "unrecognized argument: %s", argv[i]);
      return FALSE;
    }

    command_seen = TRUE;
  }

  *out_command = command;
  return TRUE;
}

static char *
application_target_output_from_command_line(GApplicationCommandLine *command_line)
{
  static const char *env_vars[] = {
    "BM_SIDEBAR_OUTPUT",
    "WAYBAR_OUTPUT_NAME",
    NULL,
  };

  for (guint i = 0; env_vars[i]; i++) {
    g_autofree char *normalized = target_output_normalize(g_application_command_line_getenv(command_line, env_vars[i]));
    if (normalized)
      return g_steal_pointer(&normalized);
  }

  return NULL;
}

static int
application_handle_command_line(Application *self,
                                SidebarCommand command,
                                const char *target_output_name,
                                gboolean is_remote)
{
  g_autoptr(GError) error = NULL;

  if (is_remote && !application_services_ensure_command_server(self, &error)) {
    g_printerr("%s: Unable to repair command socket: %s\n",
               COMMAND_NAME,
               error ? error->message : "unknown error");
    return 1;
  }

  if (command == SIDEBAR_COMMAND_BACKGROUND)
    return 0;

  if (!is_remote && sidebar_command_is_ipc_only(command)) {
    g_printerr("%s: no running instance reached for --%s\n",
               COMMAND_NAME,
               sidebar_command_to_string(command));
    g_application_quit(G_APPLICATION(self));
    return 1;
  }

  application_commands_handle_ipc(command, target_output_name, self);
  return (self->unsupported_layer_shell || self->runtime_failure) ? 1 : 0;
}

static int
application_command_line(GApplication *application, GApplicationCommandLine *command_line)
{
  Application *self = APPLICATION(application);
  g_auto(GStrv) argv = NULL;
  int argc = 0;
  SidebarCommand command = SIDEBAR_COMMAND_TOGGLE;
  g_autofree char *target_output_name = NULL;
  g_autoptr(GError) error = NULL;

  argv = g_application_command_line_get_arguments(command_line, &argc);
  if (!application_command_from_arguments(argv, argc, &command, &error)) {
    g_printerr("%s: error: %s\n", COMMAND_NAME, error ? error->message : "invalid arguments");
    return 2;
  }

  target_output_name = application_target_output_from_command_line(command_line);
  return application_handle_command_line(self,
                                         command,
                                         target_output_name,
                                         g_application_command_line_get_is_remote(command_line));
}

Application *
application_new(gboolean start_hidden)
{
  Application *self;

  g_set_application_name(APP_NAME);
  g_set_prgname(COMMAND_NAME);

  self = g_object_new(TYPE_APPLICATION,
                      "application-id", APP_ID,
                      "flags", G_APPLICATION_HANDLES_COMMAND_LINE,
                      NULL);
  self->start_hidden = start_hidden;
  self->target_output_name = target_output_from_environment();

  return self;
}

gboolean
application_get_unsupported_layer_shell(Application *self)
{
  g_return_val_if_fail(IS_APPLICATION(self), FALSE);
  return self->unsupported_layer_shell;
}

gboolean
application_get_runtime_failure(Application *self)
{
  g_return_val_if_fail(IS_APPLICATION(self), FALSE);
  return self->runtime_failure;
}

void
application_set_target_output_name(Application *self, const char *target_output_name)
{
  g_autofree char *normalized = NULL;

  g_return_if_fail(IS_APPLICATION(self));

  normalized = target_output_normalize(target_output_name);
  if (!normalized)
    return;

  g_free(self->target_output_name);
  self->target_output_name = g_steal_pointer(&normalized);
  if (self->window)
    sidebar_window_set_target_output(self->window, self->target_output_name);
}
