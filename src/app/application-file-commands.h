#pragma once

#include "app/application.h"

void application_commands_send_file_to_device(Application *self,
                                              const char *device_path,
                                              const char *file_path);
