#include <kd/vk/utils.h>
#include <kd/glfw/glfw.h>
#include <kd/glfw/window.h>
#include <vulkan/vulkan_core.h>

#include <stdio.h>
#include <string.h>

void _kd_vk_renderer_create_instance(kd_vk_renderer* rndr, VkInstance* instance) {
  const char* extensions[10];
  uint32_t extensionsCount = 0;
  if (rndr->rndr.win->api == KD_WINDOW_API_GLFW) {
    const char** req_extensions = glfwGetRequiredInstanceExtensions(&extensionsCount);
    for (uint32_t i = 0; i < extensionsCount; i++) {
      extensions[i] = req_extensions[i];
    }
  }
  extensions[extensionsCount] = "VK_EXT_debug_utils";
  extensionsCount++;

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

  if (vkCreateInstance(&instanceCreateInfo, NULL, instance) != VK_SUCCESS) {
    puts("failed to create vulkan instance");
  }
}

const char* _kd_vk_renderer_msg_severity_to_str(VkDebugUtilsMessageSeverityFlagBitsEXT bits) {
  if (bits & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    return "Error";
  } else if (bits & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    return "Warning";
  } else if (bits & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
    return "Info";
  } else if (bits & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
    return "Verbose";
  } else {
    return "Unknown";
  }
}

const char* _kd_vk_renderer_msg_type_to_str(VkDebugUtilsMessageTypeFlagBitsEXT type) {
  if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
    return "Performance";
  } else if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
    return "Validation";
  } else if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
    return "General";
  } else {
    return "Unknown";
  }
}

 VKAPI_ATTR VkBool32 VKAPI_CALL _kd_vk_renderer_debug_callback(
  VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,
  VkDebugUtilsMessageTypeFlagBitsEXT msgType,
  const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
  void* userData
) {
  printf("Validation Layer [%s | %s]: %s\n", 
    _kd_vk_renderer_msg_severity_to_str(msgSeverity),
    _kd_vk_renderer_msg_type_to_str(msgType),
    callbackData->pMessage
  );
  return VK_FALSE;
}

void _kd_vk_renderer_create_debug_messenger(kd_vk_renderer* rndr, VkInstance instance, VkDebugUtilsMessengerEXT* messenger) {
  VkDebugUtilsMessengerCreateInfoEXT dumCreateInfo = {};
  dumCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  dumCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
  dumCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT; 
  dumCreateInfo.pfnUserCallback = _kd_vk_renderer_debug_callback;
  dumCreateInfo.pUserData = NULL;

  PFN_vkCreateDebugUtilsMessengerEXT _vk_create_dum = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
  
  if (_vk_create_dum(instance, &dumCreateInfo, NULL, messenger) != VK_SUCCESS) {
    puts("failed to create vulkan debug messenger");
  }
}

void _kd_vk_renderer_create_surface(kd_vk_renderer* rndr, VkInstance instance, VkSurfaceKHR* surface) {
  if (rndr->rndr.win->api == KD_WINDOW_API_GLFW) {
    if (glfwCreateWindowSurface(instance, ((kd_glfw_window*)rndr->rndr.win)->handle, NULL, surface)) {
      puts("failed to create vulkan surface");
    }
  }
  else {
    puts("failed to create vulkan surface (window api not recognized)");
  }
}

void _kd_vk_renderer_choose_physical_device(kd_vk_renderer* rndr, VkInstance instance, VkSurfaceKHR surface, kd_vk_physical_device* pdevice) {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
  VkPhysicalDevice devices[deviceCount];
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

  pdevice->pdevice = VK_NULL_HANDLE;
  for (uint32_t i = 0; i < deviceCount; i++) {
    VkPhysicalDeviceProperties deviceProperties = {};
    vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);
    VkPhysicalDeviceFeatures deviceFeatures = {};
    vkGetPhysicalDeviceFeatures(devices[i], &deviceFeatures);

    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      pdevice->pdevice = devices[i];
      pdevice->properties = deviceProperties;
      pdevice->features = deviceFeatures;
      printf("Selected device: %s\n", deviceProperties.deviceName);
      break;
    }
  }
  
  uint32_t qFamiliesCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(pdevice->pdevice, &qFamiliesCount, NULL);
  VkQueueFamilyProperties qFamilyProperties[qFamiliesCount]; 
  vkGetPhysicalDeviceQueueFamilyProperties(pdevice->pdevice, &qFamiliesCount, qFamilyProperties);
  
  pdevice->graphicsFamilyIndex = -1;
  for (int i = 0; i < qFamiliesCount; i++) {
    if (qFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      pdevice->graphicsFamilyIndex = i;
      printf("Selected graphics queue family index: %u\n", i);
      break;
    }
  }
  
  pdevice->presentFamilyIndex = -1;
  for (int i = 0; i < qFamiliesCount; i++) {
    VkBool32 presentSupported = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(pdevice->pdevice, i, surface, &presentSupported);
    if (presentSupported == VK_TRUE) {
      pdevice->presentFamilyIndex = i;
      printf("Selected present queue family index: %u\n", i);
      break;
    }
  }
}

void _kd_vk_renderer_create_device(kd_vk_renderer* rndr, kd_vk_physical_device* pdevice, VkDevice* device) {
  float qPriority = 1.0f;
  // VkDeviceQueueCreateInfo qCreateInfos[2];
  // qCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  // qCreateInfos[0].queueFamilyIndex = pdevice->graphicsFamilyIndex;
  // qCreateInfos[0].queueCount = 1;
  // qCreateInfos[0].pQueuePriorities = &qPriority;
  
  // qCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  // qCreateInfos[1].queueFamilyIndex = pdevice->presentFamilyIndex;
  // qCreateInfos[1].queueCount = 1;
  // qCreateInfos[1].pQueuePriorities = &qPriority; 
  
  VkDeviceQueueCreateInfo qCreateInfos = {};
  qCreateInfos.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  qCreateInfos.queueFamilyIndex = pdevice->graphicsFamilyIndex;
  qCreateInfos.queueCount = 1;
  qCreateInfos.pQueuePriorities = &qPriority;

  VkDeviceCreateInfo deviceCreateInfo = {};
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceCreateInfo.pQueueCreateInfos = &qCreateInfos;
  deviceCreateInfo.queueCreateInfoCount = 1;
  deviceCreateInfo.pEnabledFeatures = &pdevice->features;
  deviceCreateInfo.enabledExtensionCount = 0;
  deviceCreateInfo.enabledLayerCount = 0;

  if (vkCreateDevice(pdevice->pdevice, &deviceCreateInfo, NULL, device) != VK_SUCCESS) {
    puts("failed to create vulkan's logical device");
  }
}
