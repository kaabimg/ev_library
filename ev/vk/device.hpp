#pragma once

#include <ev/vk/common.hpp>

namespace Vk {

class PhysicalDevice : HandleWrapper<VkPhysicalDevice> {
public:
    PhysicalDevice(VkPhysicalDevice device) : Super(device)
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
};

class Device : HandleWrapper<VkDevice> {
public:
    Device():Super(){}
    Device(VkDevice device) : Super(device)
    {
    }

    ~Device()
    {
        if (m_handle) vkDestroyDevice(m_handle, nullptr);
    }

    Device(Device&& rhs) = default;
    Device& operator=(Device&& rhs) = default;
};
}
