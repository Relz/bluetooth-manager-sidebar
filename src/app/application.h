#pragma once

#include <adwaita.h>

#include "command/sidebar-command.h"

G_BEGIN_DECLS

#define TYPE_APPLICATION (application_get_type())

typedef struct _Application Application;
typedef struct _ApplicationClass ApplicationClass;

struct _ApplicationClass {
  AdwApplicationClass parent_class;
};

GType application_get_type(void);

#define APPLICATION(object) (G_TYPE_CHECK_INSTANCE_CAST((object), TYPE_APPLICATION, Application))
#define IS_APPLICATION(object) (G_TYPE_CHECK_INSTANCE_TYPE((object), TYPE_APPLICATION))

G_DEFINE_AUTOPTR_CLEANUP_FUNC(Application, g_object_unref)

gboolean application_command_from_arguments(char **argv,
                                           int argc,
                                           SidebarCommand *out_command,
                                           GError **error);
Application *application_new(gboolean start_hidden);
gboolean application_get_unsupported_layer_shell(Application *self);
gboolean application_get_runtime_failure(Application *self);

void application_show_sidebar(Application *self);
void application_hide_sidebar(Application *self);
void application_toggle_sidebar(Application *self);
void application_show_toast(Application *self, const char *message);
void application_reload_user_css(Application *self);
void application_set_target_output_name(Application *self, const char *target_output_name);

G_END_DECLS
