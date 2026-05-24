#pragma once

#include "app/application.h"

void application_commands_set_auto_accept_trusted_files(Application *self,
                                                        gboolean auto_accept);
void application_commands_set_received_files_location(Application *self,
                                                      const char *location);
