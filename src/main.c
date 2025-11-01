#include <kd/window.h>
#include <kd/renderer.h>

int main() { 
  kd_context ctx = kd_context_create();
  kd_window* win = kd_window_create(&ctx, 1024, 768, "title");
  kd_renderer* rndr = kd_renderer_create(&ctx, win, KD_RENDERER_API_VULKAN);

  kd_window_initialize(&ctx, win);
  kd_renderer_initialize(&ctx, rndr);

  while (!kd_window_closed(&ctx, win)) {
    kd_window_update(&ctx, win);
  }

  kd_renderer_destroy(&ctx, rndr);
  kd_window_destroy(&ctx, win);
  return 0;
}
