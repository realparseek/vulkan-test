#include <kd/renderer.h>
#include <kd/vk/renderer.h>

#include <stdlib.h>

kd_renderer* kd_renderer_create(kd_context* ctx, kd_window* output_win, kd_renderer_api api) {
  switch (api) {
    case KD_RENDERER_TYPE_VULKAN:
      return (kd_renderer*)kd_vk_renderer_create(ctx, output_win);
      break;
    default:
      return NULL;
  }
}

void kd_renderer_destroy(kd_context* ctx, kd_renderer* rndr) {
  switch (rndr->api) {
    case KD_RENDERER_TYPE_VULKAN:
      kd_vk_renderer_destroy(ctx, (kd_vk_renderer*)rndr);
      break;
    default:
      return;
  }
  free(rndr);
}
