#include "device.hpp"
using namespace Vk;

PhysicalDeviceFeatures::PhysicalDeviceFeatures()
{
    auto begin = reinterpret_cast<VkBool32*>(this);
    auto end = begin + (sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32));
    std::fill(begin, end, 0);
}

DeviceQueueCreateInfo::DeviceQueueCreateInfo()
{
    sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    pNext = nullptr;
    flags = 0;
    queueFamilyIndex = 0;
    queueCount = 0;
    pQueuePriorities = 0;
}

void DeviceQueueCreateInfo::setQueuesPriorities(std::vector<float> priorities)
{
    m_queuesPriorities = std::move(priorities);
    queueCount = m_queuesPriorities.size();
    pQueuePriorities = m_queuesPriorities.data();
}

DeviceCreateInfo::DeviceCreateInfo()
{
    sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    pNext = nullptr;

    flags = 0;

    queueCreateInfoCount = 0;
    pQueueCreateInfos = nullptr;

    enabledLayerCount = 0;
    ppEnabledLayerNames = 0;

    enabledExtensionCount = 0;
    ppEnabledExtensionNames = nullptr;

    pEnabledFeatures = &m_features;
}

void DeviceCreateInfo::setQueueCreateInfos(std::vector<DeviceQueueCreateInfo> queueCreateInfos)
{
    m_queueCreateInfos = std::move(queueCreateInfos);
    pQueueCreateInfos = m_queueCreateInfos.data();
    queueCreateInfoCount = m_queueCreateInfos.size();
}

void DeviceCreateInfo::setEnabledLayers(const char* const* enabledLayers, uint32_t count)
{
    enabledLayerCount = count;
    ppEnabledLayerNames = enabledLayers;
}

PhysicalDeviceFeatures& DeviceCreateInfo::enabledFeatures()
{
    return m_features;
}

LogicalDevice::LogicalDevice(VkDevice device) : WrapperType(device)
{
}

LogicalDevice::~LogicalDevice()
{
    if (m_handle) vkDestroyDevice(m_handle, nullptr);
}

PhysicalDevice::PhysicalDevice(VkPhysicalDevice device) : WrapperType(device)
{
}

VkPhysicalDeviceProperties PhysicalDevice::properties() const
{
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(m_handle, &deviceProperties);
    return deviceProperties;
}

VkPhysicalDeviceFeatures PhysicalDevice::features() const
{
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(m_handle, &deviceFeatures);
    return deviceFeatures;
}

VkPhysicalDeviceMemoryProperties PhysicalDevice::memoryProperties() const
{
    VkPhysicalDeviceMemoryProperties result;
    vkGetPhysicalDeviceMemoryProperties(m_handle, &result);
    return result;
}

uint32_t PhysicalDevice::queueFamilyCount() const
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_handle, &queueFamilyCount, nullptr);
    return queueFamilyCount;
}

std::vector<VkQueueFamilyProperties> PhysicalDevice::queueFamilies() const
{
    uint32_t count = queueFamilyCount();
    std::vector<VkQueueFamilyProperties> queueFamilies(count);
    vkGetPhysicalDeviceQueueFamilyProperties(m_handle, &count, queueFamilies.data());
    return queueFamilies;
}

LogicalDevice PhysicalDevice::createDevice(const DeviceCreateInfo &createInfo)
{
    VkDevice deviceHandle;
    call(vkCreateDevice, m_handle, &createInfo, nullptr, &deviceHandle);
    return LogicalDevice(deviceHandle);
}
