#pragma once

#include <kd/vk/renderer.h>

extern void _kd_vk_renderer_create_instance(kd_vk_renderer* rndr, VkInstance* instance);

extern const char* _kd_vk_renderer_msg_severity_to_str(VkDebugUtilsMessageSeverityFlagBitsEXT bits);

extern const char* _kd_vk_renderer_msg_type_to_str(VkDebugUtilsMessageTypeFlagBitsEXT type);

extern VKAPI_ATTR VkBool32 VKAPI_CALL _kd_vk_renderer_debug_callback(
  VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,
  VkDebugUtilsMessageTypeFlagBitsEXT msgType,
  const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
  void* userData
);

extern void _kd_vk_renderer_create_debug_messenger(kd_vk_renderer* rndr, VkInstance instance, VkDebugUtilsMessengerEXT* messenger);

extern void _kd_vk_renderer_create_surface(kd_vk_renderer* rndr, VkInstance instance, VkSurfaceKHR* surface);

extern void _kd_vk_renderer_choose_physical_device(kd_vk_renderer* rndr, VkInstance instance, kd_vk_physical_device* pdevice);
