#include <kd/window.h>
#include <kd/renderer.h>

int main() { 
  kd_context kctx = kd_context_create();
  kd_window* win = kd_window_create(&kctx, 1024, 768, "title");
  kd_renderer* rndr = kd_renderer_create(&kctx, win, KD_RENDERER_TYPE_VULKAN);

  kd_window_initialize(&kctx, win);
  kd_renderer_initialize(&kctx, rndr);

  while (!kd_window_closed(&kctx, win)) {
    kd_window_update(&kctx, win);
  }

  kd_renderer_destroy(&kctx, rndr);
  kd_window_destroy(&kctx, win);
  return 0;
}
