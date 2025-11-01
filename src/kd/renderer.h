#pragma once

#include <kd/context.h>
#include <kd/window.h>

typedef enum kd_renderer_api {
  KD_RENDERER_API_NONE = 0,
  KD_RENDERER_API_OPENGL,
  KD_RENDERER_API_VULKAN
} kd_renderer_api;

typedef struct kd_renderer {
  kd_window* win;
  kd_renderer_api api;
} kd_renderer;

extern kd_renderer* kd_renderer_create(kd_context* ctx, kd_window* output_win, kd_renderer_api api);

extern void kd_renderer_initialize(kd_context* ctx, kd_renderer* rndr);

extern void kd_renderer_destroy(kd_context* ctx, kd_renderer* rndr);
