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

extern void _kd_vk_renderer_choose_physical_device(kd_vk_renderer* rndr, VkInstance instance, VkSurfaceKHR surface, kd_vk_physical_device* pdevice);

extern void _kd_vk_renderer_create_device(kd_vk_renderer* rndr, kd_vk_physical_device* pdevice, VkDevice* device);

extern void _kd_vk_renderer_get_graphics_queue(kd_vk_renderer* rndr, kd_vk_physical_device* pdevice, VkDevice* ldevice, VkQueue* queue);

extern void _kd_vk_renderer_get_present_queue(kd_vk_renderer* rndr, kd_vk_physical_device* pdevice, VkDevice* ldevice, VkQueue* queue);

extern kd_vk_swapchain _kd_vk_renderer_get_swapchain_details(kd_vk_renderer* rndr, kd_vk_physical_device* pdevice, VkSurfaceKHR surface);

extern void _kd_vk_renderer_create_swapchain(kd_vk_renderer* rndr, kd_vk_physical_device* pdevice, VkDevice device, VkSurfaceKHR surface, kd_vk_swapchain* swapchain);

extern void _kd_vk_renderer_create_pipeline(kd_vk_renderer* rndr, VkDevice device, kd_vk_swapchain* swapchain, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, VkPipeline* pipeline);

extern void _kd_vk_renderer_create_pipeline_layout(VkDevice device, VkPipelineLayout* layout);

extern void _kd_vk_renderer_create_renderpass(VkDevice device, kd_vk_swapchain* swapchain, VkRenderPass* rndrPass);

extern void _kd_vk_renderer_create_framebuffers(VkDevice device, kd_vk_swapchain* swapchain, VkRenderPass renderPass, VkFramebuffer* framebuffers);

extern void _kd_vk_renderer_create_command_pool(VkDevice device, kd_vk_physical_device* pdevice, VkCommandPool* cmdPool);

extern void _kd_vk_renderer_allocate_command_buffer(VkDevice device, VkCommandPool cmdPool, VkCommandBuffer* cmdBuffer);
