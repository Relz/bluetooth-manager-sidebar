#pragma once

#include <glib.h>

typedef struct _BluezCacheStatus BluezCacheStatus;

BluezCacheStatus *bluez_cache_status_new(void);
void bluez_cache_status_free(BluezCacheStatus *status);

void bluez_cache_status_clear_primary_adapter(BluezCacheStatus *status);
void bluez_cache_status_set_primary_adapter(BluezCacheStatus *status, const char *path);
const char *bluez_cache_status_primary_adapter(BluezCacheStatus *status);

void bluez_cache_status_set_error(BluezCacheStatus *status, const char *message);
const char *bluez_cache_status_error(BluezCacheStatus *status);
