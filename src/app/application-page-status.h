#pragma once

#include "app/application.h"

#include "bluez/bluez-properties.h"

gboolean application_page_string_has_text(const char *value);
BluezProperties *application_page_adapter_properties(Application *self,
                                                        const char *adapter_path);
char *application_page_dup_status_message(Application *self);
