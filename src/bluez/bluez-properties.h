#pragma once

#include <glib.h>

typedef struct _BluezProperties BluezProperties;

BluezProperties *bluez_properties_new(void);
BluezProperties *bluez_properties_new_from_variant_dict(GVariant *dict);
BluezProperties *bluez_properties_copy(BluezProperties *props);
void bluez_properties_free(BluezProperties *props);

void bluez_properties_set_variant(BluezProperties *props,
                                      const char *name,
                                      GVariant *value);
void bluez_properties_remove(BluezProperties *props, const char *name);

gboolean bluez_properties_get_bool(BluezProperties *props,
                                        const char *name,
                                        gboolean fallback);
gboolean bluez_properties_lookup_bool(BluezProperties *props,
                                          const char *name,
                                          gboolean *out_value);
char *bluez_properties_dup_string(BluezProperties *props, const char *name);
char **bluez_properties_dup_strv(BluezProperties *props, const char *name);
gint bluez_properties_get_int(BluezProperties *props,
                                   const char *name,
                                   gboolean *found);
GVariant *bluez_properties_lookup(BluezProperties *props, const char *name);
