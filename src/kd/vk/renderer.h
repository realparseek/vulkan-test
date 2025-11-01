#pragma once

#include <kd/context.h>
#include <kd/renderer.h>

#include <vulkan/vulkan.h>

typedef struct kd_vk_renderer {
  kd_renderer rndr;
  VkInstance instance;
} kd_vk_renderer;

extern kd_vk_renderer* kd_vk_renderer_create(kd_context* ctx, kd_window* output_win);

extern void kd_vk_renderer_initialize(kd_context* ctx, kd_vk_renderer* rndr);

extern void kd_vk_renderer_destroy(kd_context* ctx, kd_vk_renderer* rndr);
