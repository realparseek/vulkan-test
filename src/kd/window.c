#include <kd/window.h>
#include <kd/glfw/window.h>

kd_window* kd_window_create(kd_context* ctx, uint32_t width, uint32_t height, const char* title) {
  return (kd_window*)kd_glfw_window_create(ctx, width, height, title);
}

void kd_window_destroy(kd_context* ctx, kd_window* win) {
  switch (win->api) {
    case KD_WINDOW_API_GLFW:
      kd_glfw_window_destroy(ctx, (kd_glfw_window*)win);
    default:
      return;
  }
}

int8_t kd_window_closed(kd_context* ctx, kd_window* win) {
  switch (win->api) {
    case KD_WINDOW_API_GLFW:
      return kd_glfw_window_closed(ctx, (kd_glfw_window*)win);
    default:
      return 1;
  }
}

void kd_window_update(kd_context* ctx, kd_window* win) {
  switch (win->api) {
    case KD_WINDOW_API_GLFW:
      kd_glfw_window_update(ctx, (kd_glfw_window*)win);
    default:
      return;
  }
}
