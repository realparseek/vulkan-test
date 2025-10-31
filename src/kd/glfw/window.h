#pragma once

#include <kd/window.h>

#if !defined(GLFW_INCLUDE_NONE)
  #define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

typedef struct kd_glfw_window {
  kd_window win;
  GLFWwindow* handle;
} kd_glfw_window;

extern kd_glfw_window* kd_glfw_window_create(kd_context* ctx, uint32_t width, uint32_t height, const char* title);

extern void kd_glfw_window_initialize(kd_context* ctx, kd_glfw_window* win);

extern void kd_glfw_window_destroy(kd_context* ctx, kd_glfw_window* kwin);

extern int8_t kd_glfw_window_closed(kd_context* ctx, kd_glfw_window* kwin);

extern void kd_glfw_window_update(kd_context* ctx, kd_glfw_window* kwin);
