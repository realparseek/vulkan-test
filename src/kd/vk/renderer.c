#include <kd/vk/renderer.h>

#include <stdlib.h>

kd_vk_renderer* kd_vk_renderer_create(kd_context* ctx, kd_window* output_win) {
  kd_vk_renderer* rndr = malloc(sizeof(kd_vk_renderer));
  rndr->instance = NULL;
  rndr->rndr.api = KD_RENDERER_TYPE_VULKAN;
  rndr->rndr.win = output_win;
  return rndr;
}

void kd_vk_renderer_destroy(kd_context* ctx, kd_vk_renderer* rndr) {
  
}
