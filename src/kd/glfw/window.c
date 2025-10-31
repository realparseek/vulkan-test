#include <kd/glfw/window.h>

#include <stdlib.h>
#include <string.h>

kd_glfw_window* kd_glfw_window_create(kd_context* ctx, uint32_t width, uint32_t height, const char* title) {
  kd_glfw_window* win = malloc(sizeof(kd_glfw_window));

  strncpy(win->win.title, title, sizeof(win->win.title));
  win->win.width = width;
  win->win.height = height;
  win->handle = NULL;
  win->win.api = KD_WINDOW_API_GLFW;

  return win;
}

void kd_glfw_window_initialize(kd_context* ctx, kd_glfw_window* win) {
  if (!ctx->window_api_initialized) {
    glfwInit();
    ctx->window_api_initialized = 1;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); 

  GLFWwindow* gwindow = glfwCreateWindow((int)win->win.width, (int)win->win.height, win->win.title, NULL, NULL);

  /* *insert a fucking shit ton of error handling* */
  
  win->handle = gwindow;
  ctx->windows_count++;
}

void kd_glfw_window_destroy(kd_context* ctx, kd_glfw_window* kwin) {
  glfwDestroyWindow(kwin->handle);
  glfwTerminate();
  ctx->window_api_initialized = 0;
  memset(kwin, 0, sizeof(kd_window));
  ctx->windows_count--;
}

int8_t kd_glfw_window_closed(kd_context* ctx, kd_glfw_window* kwin) {
  return glfwWindowShouldClose(kwin->handle);
}

void kd_glfw_window_update(kd_context* ctx, kd_glfw_window* kwin) {
  glfwPollEvents();
}
