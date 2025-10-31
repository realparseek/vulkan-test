#include <kd/glfw/window.h>

#include <stdlib.h>
#include <string.h>

kd_glfw_window* kd_glfw_window_create(kd_context* ctx, uint32_t width, uint32_t height, const char* title) {
  if (!ctx->window_api_initialized) {
    glfwInit();
    ctx->window_api_initialized = 1;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  kd_glfw_window* kwin = malloc(sizeof(kd_glfw_window));

  strncpy(kwin->win.title, title, sizeof(kwin->win.title));
  kwin->win.width = width;
  kwin->win.height = height;
  GLFWwindow* gwindow = glfwCreateWindow((int)width, (int)height, kwin->win.title, NULL, NULL);
  kwin->handle = gwindow;
  kwin->win.api = KD_WINDOW_API_GLFW;

  return kwin;
}

void kd_glfw_window_destroy(kd_context* ctx, kd_glfw_window* kwin) {
  glfwDestroyWindow(kwin->handle);
  glfwTerminate();
  ctx->window_api_initialized = 0;
  memset(kwin, 0, sizeof(kd_window));
}

int8_t kd_glfw_window_closed(kd_context* ctx, kd_glfw_window* kwin) {
  return glfwWindowShouldClose(kwin->handle);
}

void kd_glfw_window_update(kd_context* ctx, kd_glfw_window* kwin) {
  glfwPollEvents();
}
