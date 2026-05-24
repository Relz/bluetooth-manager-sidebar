#include "app/application-styles.h"

#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

#include "app/app-metadata.h"
#include "core/asset-paths.h"

typedef struct {
  const char *css_path;
  gboolean required;
  gboolean failed;
} CssLoadData;

typedef enum {
  CSS_LOAD_FAILED,
  CSS_LOAD_MISSING,
  CSS_LOAD_LOADED,
} CssLoadStatus;

static GtkCssProvider *user_css_provider = NULL;
static GdkDisplay *user_css_display = NULL;

static void application_report_css_problem(gboolean required, const char *message);
static CssLoadStatus application_load_css_provider(const char *css_path,
                                                      gboolean required,
                                                      GtkCssProvider **provider_out);

gboolean
application_styles_install(Application *self)
{
  GdkDisplay *display = gdk_display_get_default();
  g_autofree char *css_path = NULL;
  GtkCssProvider *provider = NULL;

  g_return_val_if_fail(IS_APPLICATION(self), FALSE);

  if (!display)
    return TRUE;

  css_path = paths_css_path();
  if (application_load_css_provider(css_path, TRUE, &provider) != CSS_LOAD_LOADED)
    return FALSE;
  gtk_style_context_add_provider_for_display(display,
                                             GTK_STYLE_PROVIDER(provider),
                                             GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  g_object_unref(provider);

  application_reload_user_css(self);
  return TRUE;
}

static void
application_report_css_problem(gboolean required, const char *message)
{
  g_printerr("%s: %s%s\n", COMMAND_NAME, required ? "" : "warning: ", message);
}

static void
application_on_css_parsing_error(GtkCssProvider *provider,
                                 GtkCssSection *section,
                                 const GError *error,
                                 gpointer user_data)
{
  CssLoadData *data = user_data;

  (void)provider;
  (void)section;

  if (data->failed)
    return;

  data->failed = TRUE;
  g_autofree char *message = data->required
                               ? g_strdup_printf("failed to load CSS stylesheet %s: %s",
                                                 data->css_path,
                                                 error ? error->message : "unknown error")
                               : g_strdup_printf("failed to load user CSS stylesheet %s: %s",
                                                 data->css_path,
                                                 error ? error->message : "unknown error");
  application_report_css_problem(data->required, message);
}

static CssLoadStatus
application_load_css_provider(const char *css_path,
                              gboolean required,
                              GtkCssProvider **provider_out)
{
  struct stat css_stat;
  GtkCssProvider *provider = NULL;
  gulong parsing_error_handler;
  CssLoadData load_data = { 0 };

  g_return_val_if_fail(provider_out != NULL, CSS_LOAD_FAILED);
  *provider_out = NULL;

  if (stat(css_path, &css_stat) != 0) {
    int saved_errno = errno;
    g_autofree char *message = NULL;

    if (!required && saved_errno == ENOENT)
      return CSS_LOAD_MISSING;

    message = required ? g_strdup_printf("CSS stylesheet not found: %s", css_path)
                       : g_strdup_printf("cannot inspect user CSS stylesheet %s: %s",
                                         css_path,
                                         g_strerror(saved_errno));
    application_report_css_problem(required, message);
    return CSS_LOAD_FAILED;
  }

  if (!S_ISREG(css_stat.st_mode)) {
    g_autofree char *message = required ? g_strdup_printf("CSS stylesheet is not a regular file: %s", css_path)
                                        : g_strdup_printf("user CSS stylesheet is not a regular file: %s", css_path);
    application_report_css_problem(required, message);
    return CSS_LOAD_FAILED;
  }

  if (access(css_path, R_OK) != 0) {
    g_autofree char *message = required ? g_strdup_printf("cannot read CSS stylesheet %s: %s", css_path, g_strerror(errno))
                                        : g_strdup_printf("cannot read user CSS stylesheet %s: %s", css_path, g_strerror(errno));
    application_report_css_problem(required, message);
    return CSS_LOAD_FAILED;
  }

  provider = gtk_css_provider_new();
  load_data.css_path = css_path;
  load_data.required = required;
  parsing_error_handler = g_signal_connect(provider,
                                           "parsing-error",
                                           G_CALLBACK(application_on_css_parsing_error),
                                           &load_data);
  gtk_css_provider_load_from_path(provider, css_path);
  g_signal_handler_disconnect(provider, parsing_error_handler);
  if (load_data.failed) {
    g_object_unref(provider);
    return CSS_LOAD_FAILED;
  }

  *provider_out = provider;
  return CSS_LOAD_LOADED;
}

void
application_styles_clear_user_css_provider(void)
{
  if (user_css_provider && user_css_display)
    gtk_style_context_remove_provider_for_display(user_css_display, GTK_STYLE_PROVIDER(user_css_provider));
  g_clear_object(&user_css_provider);
  g_clear_object(&user_css_display);
}

void
application_reload_user_css(Application *self)
{
  GdkDisplay *display = gdk_display_get_default();
  g_autofree char *css_path = NULL;
  GtkCssProvider *provider = NULL;
  CssLoadStatus status;

  g_return_if_fail(IS_APPLICATION(self));

  if (!display)
    return;

  css_path = paths_user_css_path();
  status = application_load_css_provider(css_path, FALSE, &provider);
  if (status == CSS_LOAD_MISSING) {
    application_styles_clear_user_css_provider();
    return;
  }
  if (status != CSS_LOAD_LOADED)
    return;

  application_styles_clear_user_css_provider();
  gtk_style_context_add_provider_for_display(display,
                                             GTK_STYLE_PROVIDER(provider),
                                             GTK_STYLE_PROVIDER_PRIORITY_USER);
  user_css_provider = provider;
  user_css_display = g_object_ref(display);
}
