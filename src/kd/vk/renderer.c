#include <kd/vk/renderer.h>
#include <kd/vk/utils.h>
#include <kd/glfw/window.h>

#include <stdlib.h>
#include <string.h>

kd_vk_renderer* kd_vk_renderer_create(kd_context* ctx, kd_window* output_win) {
  kd_vk_renderer* rndr = malloc(sizeof(kd_vk_renderer));
  rndr->instance = NULL;
  rndr->rndr.api = KD_RENDERER_API_VULKAN;
  rndr->rndr.win = output_win;
  return rndr;
}

void kd_vk_renderer_initialize(kd_context* ctx, kd_vk_renderer* rndr) { 
  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;

  _kd_vk_renderer_create_instance(rndr, &instance);
  _kd_vk_renderer_create_debug_messenger(rndr, instance, &debugMessenger);

  rndr->instance = instance;
  rndr->debugMessenger = debugMessenger;
}

void kd_vk_renderer_destroy(kd_context* ctx, kd_vk_renderer* rndr) {
  PFN_vkDestroyDebugUtilsMessengerEXT _vk_destroy_dum = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(rndr->instance, "vkDestroyDebugUtilsMessengerEXT");
  _vk_destroy_dum(rndr->instance, rndr->debugMessenger, NULL); 
  vkDestroyInstance(rndr->instance, NULL);
}
