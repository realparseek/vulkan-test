#include <kd/vk/utils.h>
#include <kd/glfw/glfw.h>
#include <kd/glfw/window.h>
#include <kd/utils/file/file.h>

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
  dumCreateInfo.messageSeverity = 
    // VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | 
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
    // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
    ;
  dumCreateInfo.messageType = 
    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | 
    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
    ;
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

    uint32_t deviceExtensionCount = 0;
    vkEnumerateDeviceExtensionProperties(devices[0], NULL, &deviceExtensionCount, NULL);
    VkExtensionProperties deviceExtensions[deviceExtensionCount]; 
    vkEnumerateDeviceExtensionProperties(devices[0], NULL, &deviceExtensionCount, deviceExtensions);
  
    int8_t swapchainSupport = 0;
    for (int i = 0; i < deviceExtensionCount; i++) {
      if (strcmp(deviceExtensions[i].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
        swapchainSupport = 1;
        break;
      }
    }

    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && swapchainSupport) {
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
  VkDeviceQueueCreateInfo qCreateInfos[2] = { {}, {} };
  qCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  qCreateInfos[0].queueFamilyIndex = pdevice->graphicsFamilyIndex;
  qCreateInfos[0].queueCount = 1;
  qCreateInfos[0].pQueuePriorities = &qPriority;
  
  qCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  qCreateInfos[1].queueFamilyIndex = pdevice->presentFamilyIndex;
  qCreateInfos[1].queueCount = 1;
  qCreateInfos[1].pQueuePriorities = &qPriority; 
  
  const char* extensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
  };

  VkDeviceCreateInfo deviceCreateInfo = {};
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceCreateInfo.pQueueCreateInfos = qCreateInfos;
  if (pdevice->graphicsFamilyIndex == pdevice->presentFamilyIndex)
    deviceCreateInfo.queueCreateInfoCount = 1;
  else
    deviceCreateInfo.queueCreateInfoCount = 2;
  deviceCreateInfo.pEnabledFeatures = &pdevice->features;
  deviceCreateInfo.ppEnabledExtensionNames = extensions;
  deviceCreateInfo.enabledExtensionCount = 1;
  deviceCreateInfo.enabledLayerCount = 0;

  if (vkCreateDevice(pdevice->pdevice, &deviceCreateInfo, NULL, device) != VK_SUCCESS) {
    puts("failed to create vulkan's logical device");
  }
}

void _kd_vk_renderer_get_graphics_queue(kd_vk_renderer* rndr, kd_vk_physical_device* pdevice, VkDevice* ldevice, VkQueue* queue) {
  vkGetDeviceQueue(*ldevice, pdevice->graphicsFamilyIndex, 0, queue);
}

void _kd_vk_renderer_get_present_queue(kd_vk_renderer* rndr, kd_vk_physical_device* pdevice, VkDevice* ldevice, VkQueue* queue) {
  vkGetDeviceQueue(*ldevice, pdevice->presentFamilyIndex, 0, queue);
}

kd_vk_swapchain _kd_vk_renderer_get_swapchain_details(kd_vk_renderer* rndr, kd_vk_physical_device* pdevice, VkSurfaceKHR surface) {
  kd_vk_swapchain ksc = {};
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pdevice->pdevice, surface, &ksc.capabilities);
  
  // selecting surface's format
  uint32_t formatCount = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(pdevice->pdevice, surface, &formatCount, NULL);
  VkSurfaceFormatKHR formats[formatCount];
  vkGetPhysicalDeviceSurfaceFormatsKHR(pdevice->pdevice, surface, &formatCount, formats);

  VkSurfaceFormatKHR selectedFormat = {};
  for (uint32_t i = 0; i < formatCount; i++) {
    if (formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      selectedFormat = formats[i];
      break;
    }
  }
  if (!selectedFormat.format) {
    selectedFormat = formats[0];
  }
  
  // selecting present mode
  uint32_t presentModeCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(pdevice->pdevice, surface, &presentModeCount, NULL);
  VkPresentModeKHR presentModes[presentModeCount];
  vkGetPhysicalDeviceSurfacePresentModesKHR(pdevice->pdevice, surface, &presentModeCount, presentModes);

  VkPresentModeKHR selectedPresentMode = 0;
  for (uint32_t i = 0; i < presentModeCount; i++) {
    if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
      selectedPresentMode = presentModes[i];
      break;
    }
  }
  if (!selectedPresentMode) {
    selectedPresentMode = VK_PRESENT_MODE_FIFO_KHR;
  }

  // getting extent
  VkExtent2D selectedExtent = {};
  if (ksc.capabilities.currentExtent.width != UINT32_MAX) {
    selectedExtent = ksc.capabilities.currentExtent;
  } else if (rndr->rndr.win->api == KD_WINDOW_API_GLFW) {
    int fwidth, fheight;
    glfwGetFramebufferSize(((kd_glfw_window*)rndr->rndr.win)->handle, &fwidth, &fheight);

    selectedExtent.width = (uint32_t)fwidth;
    selectedExtent.height = (uint32_t)fheight;
    uint32_t minWidth = ksc.capabilities.minImageExtent.width;
    uint32_t maxWidth = ksc.capabilities.maxImageExtent.width;
    uint32_t minHeight = ksc.capabilities.minImageExtent.height;
    uint32_t maxHeight = ksc.capabilities.maxImageExtent.height;
    
    // clamping width and height values to supported ones
    selectedExtent.width = (selectedExtent.width < minWidth ? minWidth : selectedExtent.width);
    selectedExtent.width = (selectedExtent.width > maxWidth ? maxWidth : selectedExtent.width);
    selectedExtent.height = (selectedExtent.height < minHeight ? minHeight : selectedExtent.height);
    selectedExtent.height = (selectedExtent.height > maxHeight ? maxHeight : selectedExtent.height);
  }

  uint32_t selectedImageCount = ksc.capabilities.minImageCount + 1;
  if (ksc.capabilities.minImageCount > 0 && selectedImageCount > ksc.capabilities.maxImageCount) {
    selectedImageCount = ksc.capabilities.maxImageCount;
  }

  ksc.format = selectedFormat;
  ksc.presentMode = selectedPresentMode;
  ksc.extent = selectedExtent;
  ksc.imageCount = selectedImageCount;
  return ksc;
}

void _kd_vk_renderer_create_swapchain(kd_vk_renderer* rndr, kd_vk_physical_device* pdevice, VkDevice device, VkSurfaceKHR surface, kd_vk_swapchain* swapchain) {
  *swapchain = _kd_vk_renderer_get_swapchain_details(rndr, pdevice, surface);

  VkSwapchainCreateInfoKHR scCreateInfo = {};
  scCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  scCreateInfo.imageFormat = swapchain->format.format;
  scCreateInfo.imageColorSpace = swapchain->format.colorSpace;
  scCreateInfo.presentMode = swapchain->presentMode;
  scCreateInfo.imageExtent = swapchain->extent;
  scCreateInfo.clipped = VK_TRUE;
  scCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  scCreateInfo.preTransform = swapchain->capabilities.currentTransform;
  scCreateInfo.surface = surface;
  scCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  scCreateInfo.minImageCount = swapchain->imageCount;
  scCreateInfo.imageArrayLayers = 1;
  scCreateInfo.oldSwapchain = VK_NULL_HANDLE;

  uint32_t queueFamilyIndices[] = { pdevice->graphicsFamilyIndex, pdevice->presentFamilyIndex };
  if (pdevice->graphicsFamilyIndex == pdevice->presentFamilyIndex) {
    scCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    scCreateInfo.queueFamilyIndexCount = 0;
    scCreateInfo.pQueueFamilyIndices = NULL;
  } else {
    scCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    scCreateInfo.queueFamilyIndexCount = 2;
    scCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
  }

  if (vkCreateSwapchainKHR(device, &scCreateInfo, NULL, &swapchain->swapchain) != VK_SUCCESS) {
    puts("failed to create vulkan's swapchain");
  }
  
  vkGetSwapchainImagesKHR(device, swapchain->swapchain, &swapchain->imageCount, NULL); 
  if (swapchain->imageCount > 6) {
    puts("Swapchain's images not created");
    return;
  }
  vkGetSwapchainImagesKHR(device, swapchain->swapchain, &swapchain->imageCount, swapchain->images);

  VkImageViewCreateInfo ivCreateInfo = {};
  ivCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  ivCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  ivCreateInfo.format = swapchain->format.format;
  ivCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  ivCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  ivCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  ivCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  ivCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  ivCreateInfo.subresourceRange.baseArrayLayer = 0;
  ivCreateInfo.subresourceRange.layerCount = 1;
  ivCreateInfo.subresourceRange.baseMipLevel = 0;
  ivCreateInfo.subresourceRange.levelCount = 1;

  for (uint32_t i = 0; i < swapchain->imageCount; i++) {
    ivCreateInfo.image = swapchain->images[i];
    vkCreateImageView(device, &ivCreateInfo, NULL, &swapchain->imageViews[i]);
  }
}

void _kd_vk_renderer_create_pipeline(kd_vk_renderer* rndr, VkDevice device, kd_vk_swapchain* swapchain, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, VkPipeline* pipeline) {
  uint32_t vShaderSize = kd_file_size("build/obj/kd/vk/shaders/default_vert.spv", KD_FILE_TYPE_BINARY);
  char vShader[vShaderSize+1];
  kd_file_read("build/obj/kd/vk/shaders/default_vert.spv", KD_FILE_TYPE_BINARY, vShader, vShaderSize);

  uint32_t fShaderSize = kd_file_size("build/obj/kd/vk/shaders/default_frag.spv", KD_FILE_TYPE_BINARY);
  char fShader[fShaderSize+1];
  kd_file_read("build/obj/kd/vk/shaders/default_frag.spv", KD_FILE_TYPE_BINARY, fShader, fShaderSize);

  VkShaderModuleCreateInfo shaderModuleVert = {};
  shaderModuleVert.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shaderModuleVert.codeSize = vShaderSize;
  shaderModuleVert.pCode = (const uint32_t*)vShader;

  VkShaderModule vShaderModule = {};
  if (vkCreateShaderModule(device, &shaderModuleVert, NULL, &vShaderModule) != VK_SUCCESS) {
    puts("failed to create vertex shader module");
  }

  VkShaderModuleCreateInfo shaderModuleFrag = {};
  shaderModuleFrag.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shaderModuleFrag.codeSize = fShaderSize;
  shaderModuleFrag.pCode = (const uint32_t*)fShader;

  VkShaderModule fShaderModule = {};
  if (vkCreateShaderModule(device, &shaderModuleFrag, NULL, &fShaderModule) != VK_SUCCESS) {
    puts("failed to create fragment shader module");
  }

  VkPipelineShaderStageCreateInfo shaderStageVert = {};
  shaderStageVert.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStageVert.stage = VK_SHADER_STAGE_VERTEX_BIT;
  shaderStageVert.module = vShaderModule;
  shaderStageVert.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStageFrag = {};
  shaderStageFrag.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStageFrag.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  shaderStageFrag.module = fShaderModule;
  shaderStageFrag.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages[2] = { shaderStageVert, shaderStageFrag };

  VkDynamicState dStates[2] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
  };

  VkPipelineDynamicStateCreateInfo dynamicState = {};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.pDynamicStates = dStates;
  dynamicState.dynamicStateCount = 2;

  VkPipelineVertexInputStateCreateInfo vertexInput = {};
  vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInput.pVertexAttributeDescriptions = NULL;
  vertexInput.vertexAttributeDescriptionCount = 0;
  vertexInput.pVertexBindingDescriptions = NULL;
  vertexInput.vertexBindingDescriptionCount = 0;

  VkPipelineInputAssemblyStateCreateInfo inputAss = {};
  inputAss.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAss.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAss.primitiveRestartEnable = VK_FALSE;
  
  VkViewport viewport = {};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  viewport.width = swapchain->extent.width;
  viewport.height = swapchain->extent.height;

  VkRect2D scissor = {};
  scissor.offset = (VkOffset2D){0, 0};
  scissor.extent = swapchain->extent;

  VkPipelineViewportStateCreateInfo viewportState = {};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.pViewports = &viewport;
  viewportState.viewportCount = 1;
  viewportState.pScissors = &scissor;
  viewportState.scissorCount = 1;

  VkPipelineRasterizationStateCreateInfo rasterizationState = {};
  rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizationState.depthClampEnable = VK_FALSE;
  rasterizationState.rasterizerDiscardEnable = VK_FALSE;
  rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizationState.lineWidth = 1.0f;
  rasterizationState.cullMode = VK_CULL_MODE_NONE;
  rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizationState.depthBiasEnable = VK_FALSE;
  rasterizationState.depthBiasClamp = 0.0f;
  rasterizationState.depthBiasConstantFactor = 0.0f;
  rasterizationState.depthBiasSlopeFactor = 0.0f;

  VkPipelineMultisampleStateCreateInfo multisampleState = {};
  multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampleState.sampleShadingEnable = VK_FALSE;
  multisampleState.alphaToCoverageEnable = VK_FALSE;
  multisampleState.alphaToOneEnable = VK_FALSE;
  multisampleState.minSampleShading = 1.0f;
  multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
  colorBlendAttachment.colorWriteMask = 
    VK_COLOR_COMPONENT_R_BIT | 
    VK_COLOR_COMPONENT_G_BIT | 
    VK_COLOR_COMPONENT_B_BIT | 
    VK_COLOR_COMPONENT_A_BIT; 
  colorBlendAttachment.blendEnable = VK_FALSE;
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineColorBlendStateCreateInfo colorBlend = {};
  colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlend.pAttachments = &colorBlendAttachment;
  colorBlend.attachmentCount = 1;
  colorBlend.logicOp = VK_FALSE;
  colorBlend.logicOp = VK_LOGIC_OP_COPY;

  VkGraphicsPipelineCreateInfo pipCreateInfo = {};
  pipCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipCreateInfo.pStages = shaderStages;
  pipCreateInfo.stageCount = 2;
  pipCreateInfo.pDynamicState = &dynamicState;
  pipCreateInfo.pVertexInputState = &vertexInput;
  pipCreateInfo.pInputAssemblyState = &inputAss;
  pipCreateInfo.pViewportState = &viewportState;
  pipCreateInfo.pRasterizationState = &rasterizationState;
  pipCreateInfo.pMultisampleState = &multisampleState;
  pipCreateInfo.pTessellationState = NULL;
  pipCreateInfo.pDepthStencilState = NULL;
  pipCreateInfo.pColorBlendState = &colorBlend;
  pipCreateInfo.layout = pipelineLayout;
  pipCreateInfo.renderPass = renderPass;
  pipCreateInfo.subpass = 0;
  pipCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipCreateInfo.basePipelineIndex = -1;

  if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipCreateInfo, NULL, pipeline) != VK_SUCCESS) {
    puts("failed to create vulkan's graphics pipeline");
  }
  
  vkDestroyShaderModule(device, vShaderModule, NULL);
  vkDestroyShaderModule(device, fShaderModule, NULL);
}

void _kd_vk_renderer_create_pipeline_layout(VkDevice device, VkPipelineLayout* layout) {
  VkPipelineLayoutCreateInfo pipLayoutInfo = {};
  pipLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipLayoutInfo.setLayoutCount = 0;
  pipLayoutInfo.pSetLayouts = NULL;
  pipLayoutInfo.pushConstantRangeCount = 0;
  pipLayoutInfo.pPushConstantRanges = NULL;
  
  if (vkCreatePipelineLayout(device, &pipLayoutInfo, NULL, layout) != VK_SUCCESS) {
    puts("failed to create pipeline layout");
  }
}

void _kd_vk_renderer_create_renderpass(VkDevice device, kd_vk_swapchain* swapchain, VkRenderPass* rndrPass) {
  VkAttachmentDescription attachment = {}; 
  attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  attachment.format = swapchain->format.format;

  VkAttachmentReference colorAttachmentRef = {};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = {};
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  VkRenderPassCreateInfo renderPass = {};
  renderPass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPass.pSubpasses = &subpass;
  renderPass.subpassCount = 1;
  renderPass.pAttachments = &attachment;
  renderPass.attachmentCount = 1;

  if (vkCreateRenderPass(device, &renderPass, NULL, rndrPass) != VK_SUCCESS) {
    puts("failed to create render pass");
  }
}

void _kd_vk_renderer_create_framebuffers(VkDevice device, kd_vk_swapchain* swapchain, VkRenderPass renderPass, VkFramebuffer* framebuffers) {
  VkFramebufferCreateInfo fbCreateInfo = {};
  fbCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  fbCreateInfo.width = swapchain->extent.width;
  fbCreateInfo.height = swapchain->extent.height;
  fbCreateInfo.renderPass = renderPass;
  fbCreateInfo.layers = 1;

  for (uint32_t f = 0; f < swapchain->imageCount; f++) {
    fbCreateInfo.pAttachments = &swapchain->imageViews[f];
    fbCreateInfo.attachmentCount = 1;

    if (vkCreateFramebuffer(device, &fbCreateInfo, NULL, &framebuffers[f]) != VK_SUCCESS) {
      printf("failed to create framebuffer #%u\n", f);
    }
  }
}
