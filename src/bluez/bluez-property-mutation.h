#pragma once

#include "bluez/bluez-state.h"

typedef struct {
  const char *interface_name;
  const char *property_name;
  const char *status_message;
} BluezPropertyMutationDescriptor;

typedef void (*BluezPropertyMutationFinished)(const BluezPropertyMutationDescriptor *descriptor,
                                                const char *object_path,
                                                gboolean success,
                                                const GError *error,
                                                gpointer user_data);

gboolean bluez_property_mutation_start(BluezState *state,
                                       const char *object_path,
                                       const BluezPropertyMutationDescriptor *descriptor,
                                       GVariant *value,
                                       BluezPropertyMutationFinished finished,
                                       gpointer user_data,
                                       GDestroyNotify destroy);
