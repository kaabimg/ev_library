#pragma once

#include <ev/vk/common.hpp>

namespace Vk {

class PhysicalDevice : HandleWrapper<VkPhysicalDevice> {
public:
    PhysicalDevice(VkPhysicalDevice device) : WrapperType(device)
    {
    }

    VkPhysicalDeviceProperties properties() const
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(m_handle, &deviceProperties);
        return deviceProperties;
    }

    VkPhysicalDeviceFeatures features() const
    {
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(m_handle, &deviceFeatures);
        return deviceFeatures;
    }

    uint32_t queueFamilyCount() const
    {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(m_handle, &queueFamilyCount, nullptr);
        return queueFamilyCount;
    }
    std::vector<VkQueueFamilyProperties> queueFamilies() const
    {
        uint32_t count = queueFamilyCount();
        std::vector<VkQueueFamilyProperties> queueFamilies(count);
        vkGetPhysicalDeviceQueueFamilyProperties(m_handle, &count, queueFamilies.data());
        return queueFamilies;
    }
};

class LogicalDevice : HandleWrapper<VkDevice> {
public:
    LogicalDevice() : WrapperType()
    {
    }
    LogicalDevice(VkDevice device) : WrapperType(device)
    {
    }

    ~LogicalDevice()
    {
        if (m_handle) vkDestroyDevice(m_handle, nullptr);
    }

    LogicalDevice(LogicalDevice&& rhs) = default;
    LogicalDevice& operator=(LogicalDevice&& rhs) = default;
};
}
