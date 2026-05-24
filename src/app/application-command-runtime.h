#pragma once

#include "app/application.h"

typedef struct _ApplicationCommandRuntime ApplicationCommandRuntime;

ApplicationCommandRuntime *application_command_runtime_new(void);
gboolean application_command_runtime_start(ApplicationCommandRuntime *runtime,
                                            Application *self,
                                            GError **error);
gboolean application_command_runtime_ensure_healthy(ApplicationCommandRuntime *runtime,
                                                    Application *self,
                                                    GError **error);
void application_command_runtime_free(ApplicationCommandRuntime *runtime);
