#include "VulkanRenderer.h"

VulkanRenderer::VulkanRenderer() : instance(VK_NULL_HANDLE), physicalDevice(VK_NULL_HANDLE), device(VK_NULL_HANDLE) {}

VulkanRenderer::~VulkanRenderer() {
    Cleanup();
}

void VulkanRenderer::Initialize() {
    CreateInstance();
    SelectPhysicalDevice();
    CreateLogicalDevice();
}

void VulkanRenderer::Cleanup() {
    if (device != VK_NULL_HANDLE) {
        vkDestroyDevice(device, nullptr);
        device = VK_NULL_HANDLE;
    }

    if (instance != VK_NULL_HANDLE) {
        vkDestroyInstance(instance, nullptr);
        instance = VK_NULL_HANDLE;
    }
}

void VulkanRenderer::CreateInstance() {
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Voxploria";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "None";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
}

void VulkanRenderer::SelectPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        throw std::runtime_error("Failed to find GPUs with Vulkan support.");
    }

    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

    VkPhysicalDevice preferredDevice = VK_NULL_HANDLE;
    VkPhysicalDevice fallbackDevice = VK_NULL_HANDLE;

    for (const auto& device : physicalDevices) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        std::cout << "Physical Device: " << deviceProperties.deviceName << std::endl;

        if (deviceProperties.apiVersion >= VK_API_VERSION_1_0) {
            if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                preferredDevice = device;
                break;
            }

            if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
                fallbackDevice = device;
            }
        }
    }
    if (preferredDevice != VK_NULL_HANDLE) {
        physicalDevice = preferredDevice;
    }
    else if (fallbackDevice != VK_NULL_HANDLE) {
        physicalDevice = fallbackDevice;
    }
    else {
        throw std::runtime_error("Failed to find a suitable GPU.");
    }
}


void VulkanRenderer::CreateLogicalDevice() {
    std::vector<VkQueueFamilyProperties> queueFamilies;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    queueFamilies.resize(queueFamilyCount);

    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    int graphicsQueueFamilyIndex = -1;
    int computeQueueFamilyIndex = -1;
    int transferQueueFamilyIndex = -1;

    for (uint32_t i = 0; i < queueFamilyCount; ++i) {
        const VkQueueFamilyProperties& queueFamily = queueFamilies[i];

        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsQueueFamilyIndex = i;
        }

        if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
            computeQueueFamilyIndex = i;
        }

        if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) {
            transferQueueFamilyIndex = i;
        }

        if (graphicsQueueFamilyIndex != -1 && computeQueueFamilyIndex != -1 && transferQueueFamilyIndex != -1) {
            break;
        }
    }

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    float queuePriority = 1.0f;

    VkDeviceQueueCreateInfo graphicsQueueCreateInfo = {};
    graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    graphicsQueueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
    graphicsQueueCreateInfo.queueCount = 1;
    graphicsQueueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(graphicsQueueCreateInfo);

    VkDeviceQueueCreateInfo computeQueueCreateInfo = {};
    computeQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    computeQueueCreateInfo.queueFamilyIndex = computeQueueFamilyIndex;
    computeQueueCreateInfo.queueCount = 1;
    computeQueueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(computeQueueCreateInfo);

    VkDeviceQueueCreateInfo transferQueueCreateInfo = {};
    transferQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    transferQueueCreateInfo.queueFamilyIndex = transferQueueFamilyIndex;
    transferQueueCreateInfo.queueCount = 1;
    transferQueueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(transferQueueCreateInfo);

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

    std::vector<const char*> deviceExtensions = {};
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    VkDevice device = VK_NULL_HANDLE;
    VkResult result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create logical device.");
    }

    this->device = device;
}

