#include <kd/vk/renderer.h>
#include <kd/vk/utils.h>
#include <kd/glfw/window.h>
#include <kd/utils/file/file.h>

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
  VkSurfaceKHR surface;
  kd_vk_physical_device pdevice;
  VkDevice device;
  kd_vk_swapchain swapchain;
  VkRenderPass renderPass;
  VkPipelineLayout pipelineLayout;
  VkPipeline pipeline;

  _kd_vk_renderer_create_instance(rndr, &instance);
  _kd_vk_renderer_create_debug_messenger(rndr, instance, &debugMessenger);
  _kd_vk_renderer_create_surface(rndr, instance, &surface);
  _kd_vk_renderer_choose_physical_device(rndr, instance, surface, &pdevice);
  _kd_vk_renderer_create_device(rndr, &pdevice, &device);
  _kd_vk_renderer_get_graphics_queue(rndr, &pdevice, &device, &pdevice.graphicsQueue);
  _kd_vk_renderer_get_present_queue(rndr, &pdevice, &device, &pdevice.presentQueue);
  _kd_vk_renderer_create_swapchain(rndr, &pdevice, device, surface, &swapchain);
  _kd_vk_renderer_create_pipeline_layout(device, &pipelineLayout);
  _kd_vk_renderer_create_renderpass(device, &swapchain, &renderPass);
  _kd_vk_renderer_create_pipeline(rndr, device, &swapchain, pipelineLayout, renderPass,  &pipeline);

  rndr->instance = instance;
  rndr->debugMessenger = debugMessenger;
  rndr->surface = surface;
  rndr->pdevice = pdevice;
  rndr->device = device;
  rndr->swapchain = swapchain;
  rndr->pipelineLayout = pipelineLayout;
  rndr->renderPass = renderPass;
  rndr->pipeline = pipeline;
}

void kd_vk_renderer_destroy(kd_context* ctx, kd_vk_renderer* rndr) {
  vkDestroyPipelineLayout(rndr->device, rndr->pipelineLayout, NULL);
  vkDestroyRenderPass(rndr->device, rndr->renderPass, NULL);
  vkDestroyPipeline(rndr->device, rndr->pipeline, NULL);
  for (uint32_t iv = 0; iv < rndr->swapchain.imageCount; iv++) {
    vkDestroyImageView(rndr->device, rndr->swapchain.imageViews[iv], NULL);
  }
  vkDestroySwapchainKHR(rndr->device, rndr->swapchain.swapchain, NULL);
  vkDestroyDevice(rndr->device, NULL);
  vkDestroySurfaceKHR(rndr->instance, rndr->surface, NULL);
  PFN_vkDestroyDebugUtilsMessengerEXT _vk_destroy_dum = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(rndr->instance, "vkDestroyDebugUtilsMessengerEXT");
  _vk_destroy_dum(rndr->instance, rndr->debugMessenger, NULL); 
  vkDestroyInstance(rndr->instance, NULL);
}
