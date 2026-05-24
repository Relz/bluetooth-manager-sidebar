#pragma once

#include "bluez/bluez-operation-reporter.h"
#include "bluez/bluez-property-mutation.h"

typedef BluezState *(*BluezPropertyActionState)(gpointer user_data);
typedef gboolean (*BluezPropertyActionDisposed)(gpointer user_data);
typedef void (*BluezPropertyActionFinished)(const BluezPropertyMutationDescriptor *descriptor,
                                              const char *object_path,
                                              gboolean success,
                                              const GError *error,
                                              gboolean owner_disposed,
                                              gpointer user_data);

gboolean bluez_property_action_runner_start(BluezOperationReporter *reporter,
                                            const char *object_path,
                                            const BluezPropertyMutationDescriptor *descriptor,
                                            GVariant *value,
                                            BluezPropertyActionState state,
                                            BluezPropertyActionDisposed disposed,
                                            BluezPropertyActionFinished finished,
                                            gpointer user_data,
                                            GDestroyNotify destroy);
