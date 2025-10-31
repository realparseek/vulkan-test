#pragma once

#include <inttypes.h>

typedef struct kd_context {
  uint8_t window_api_initialized;
  uint16_t windows_count;
} kd_context;

extern kd_context kd_context_create();
