#pragma once

#include <kd/context.h>

#include <inttypes.h>

typedef enum kd_window_api {
  KD_WINDOW_API_NONE = 0,
  KD_WINDOW_API_GLFW
} kd_window_api;

typedef struct kd_window {
  uint32_t width;
  uint32_t height;
  char title[128];
  kd_window_api api;
} kd_window;

extern kd_window* kd_window_create(kd_context* ctx, uint32_t width, uint32_t height, const char* title);

extern void kd_window_destroy(kd_context* ctx, kd_window* win);

extern int8_t kd_window_closed(kd_context* ctx, kd_window* win);

extern void kd_window_update(kd_context* ctx, kd_window* win);
