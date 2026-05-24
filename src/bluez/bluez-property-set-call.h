#pragma once

#include <glib.h>

#include "bluez/bluez-state.h"

typedef void (*BluezPropertySetCallFinished)(const char *object_path,
                                               const char *interface_name,
                                               const char *property_name,
                                               gboolean success,
                                               const GError *error,
                                               gpointer user_data);

gboolean bluez_property_set_call_start(BluezState *state,
                                       const char *object_path,
                                       const char *interface_name,
                                       const char *property_name,
                                       GVariant *value,
                                       BluezPropertySetCallFinished finished,
                                       gpointer user_data,
                                       GDestroyNotify destroy);
