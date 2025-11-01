#include <kd/vk/renderer.h>

#include <vulkan/vulkan_core.h>
#include <kd/glfw/window.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void _kd_vk_renderer_create_instance(kd_vk_renderer* rndr, VkInstance* instance) {
  const char** extensions = NULL;
  uint32_t extensionsCount = 0;
  if (rndr->rndr.win->api == KD_WINDOW_API_GLFW) {
    extensions = glfwGetRequiredInstanceExtensions(&extensionsCount);
  }

  // check validation layers support
  uint32_t layerPropertiesCount = 0;
  vkEnumerateInstanceLayerProperties(&layerPropertiesCount, NULL);
  VkLayerProperties layerProperties[layerPropertiesCount];
  vkEnumerateInstanceLayerProperties(&layerPropertiesCount, layerProperties);
  
  int8_t validationLayerSupported = 0;
  for (uint32_t i = 0; i < layerPropertiesCount; i++) {
    if (strcmp(layerProperties[i].layerName, "VK_LAYER_KHRONOS_validation") == 0) {
      validationLayerSupported = 1;
      break;
    }
  }
  
  const char* enabledLayers[10];
  uint32_t enabledLayersCount = 0;
  if (validationLayerSupported) {
    enabledLayers[enabledLayersCount] = "VK_LAYER_KHRONOS_validation";
    enabledLayersCount++;
  }

  VkApplicationInfo appInfo = {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "title";
  appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
  appInfo.apiVersion = VK_API_VERSION_1_4;
  appInfo.pEngineName = NULL;
  appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);

  VkInstanceCreateInfo instanceCreateInfo = {};
  instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceCreateInfo.pApplicationInfo = &appInfo;
  instanceCreateInfo.ppEnabledExtensionNames = extensions;
  instanceCreateInfo.enabledExtensionCount = extensionsCount;
  instanceCreateInfo.ppEnabledLayerNames = enabledLayers;
  instanceCreateInfo.enabledLayerCount = enabledLayersCount;

  vkCreateInstance(&instanceCreateInfo, NULL, instance);
}

kd_vk_renderer* kd_vk_renderer_create(kd_context* ctx, kd_window* output_win) {
  kd_vk_renderer* rndr = malloc(sizeof(kd_vk_renderer));
  rndr->instance = NULL;
  rndr->rndr.api = KD_RENDERER_API_VULKAN;
  rndr->rndr.win = output_win;
  return rndr;
}

void kd_vk_renderer_initialize(kd_context* ctx, kd_vk_renderer* rndr) { 
  VkInstance instance;

  _kd_vk_renderer_create_instance(rndr, &instance);

  rndr->instance = instance;
}

void kd_vk_renderer_destroy(kd_context* ctx, kd_vk_renderer* rndr) {
  vkDestroyInstance(rndr->instance, NULL);
}
