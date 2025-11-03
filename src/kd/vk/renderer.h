#pragma once

#include <kd/context.h>
#include <kd/renderer.h>

#include <kd/vk/vulkan.h>

typedef struct kd_vk_physical_device {
  VkPhysicalDevice pdevice;
  VkPhysicalDeviceProperties properties;
  VkPhysicalDeviceFeatures features;
  uint32_t graphicsFamilyIndex;
  uint32_t presentFamilyIndex;
  VkQueue graphicsQueue;
  VkQueue presentQueue;
} kd_vk_physical_device;

typedef struct kd_vk_renderer {
  kd_renderer rndr;
  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;
  VkSurfaceKHR surface;
  kd_vk_physical_device pdevice;
  VkDevice device;
} kd_vk_renderer;

extern kd_vk_renderer* kd_vk_renderer_create(kd_context* ctx, kd_window* output_win);

extern void kd_vk_renderer_initialize(kd_context* ctx, kd_vk_renderer* rndr);

extern void kd_vk_renderer_destroy(kd_context* ctx, kd_vk_renderer* rndr);
