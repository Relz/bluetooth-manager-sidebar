#pragma once

#include "app/application.h"

void application_commands_set_adapter_power(Application *self, gboolean powered);
void application_commands_set_adapter_discoverable(Application *self, gboolean discoverable);
void application_commands_toggle_discovery(Application *self);

void application_commands_activate_device(Application *self, const char *device_path);
void application_commands_trust_device(Application *self, const char *device_path);
void application_commands_set_device_alias(Application *self,
                                           const char *device_path,
                                           const char *alias);
void application_commands_set_device_trusted(Application *self,
                                             const char *device_path,
                                             gboolean trusted);
void application_commands_set_device_blocked(Application *self,
                                             const char *device_path,
                                             gboolean blocked);
void application_commands_forget_device(Application *self, const char *device_path);
