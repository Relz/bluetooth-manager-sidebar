#pragma once

#include "app/application.h"

#include "bluez/bluez-properties.h"

char *application_device_presentation_dup_display_name(Application *self, const char *device_path);
char *application_device_presentation_dup_subtitle(Application *self, const char *device_path);
char *application_device_presentation_dup_property_display(BluezProperties *props, const char *name);
