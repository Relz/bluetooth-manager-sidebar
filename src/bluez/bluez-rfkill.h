#pragma once

#include <glib.h>

typedef struct {
  gboolean success;
  char *reason;
  char *detail;
} RfkillUnblockResult;

void rfkill_unblock_result_clear(RfkillUnblockResult *result);
gboolean rfkill_unblock_bluetooth(RfkillUnblockResult *out_result);
