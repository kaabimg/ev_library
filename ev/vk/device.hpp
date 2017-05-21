#pragma once

#include <ev/vk/common.hpp>

namespace Vk {

struct PhysicalDeviceFeatures : VkPhysicalDeviceFeatures {
    PhysicalDeviceFeatures()
    {
        auto begin = reinterpret_cast<VkBool32*>(this);
        auto end = begin + (sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32));
        std::fill(begin, end, 0);
    }
};

struct DeviceCreateInfo : VkDeviceCreateInfo {
    DeviceCreateInfo()
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

        pEnabledFeatures = nullptr;
    }

    void setQueueCreateInfos(const VkDeviceQueueCreateInfo* infos, uint32_t count)
    {
        pQueueCreateInfos = infos;
        queueCreateInfoCount = count;
    }
};

class LogicalDevice : public HandleWrapper<VkDevice> {
public:
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

class PhysicalDevice : public HandleWrapper<VkPhysicalDevice> {
public:
    PhysicalDevice(VkPhysicalDevice device) : WrapperType(device)
    {
    }

    PhysicalDevice(PhysicalDevice&&) = default;
    PhysicalDevice& operator=(PhysicalDevice&&) = default;

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

    VkPhysicalDeviceMemoryProperties memoryProperties() const
    {
        VkPhysicalDeviceMemoryProperties result;
        vkGetPhysicalDeviceMemoryProperties(m_handle, &result);
        return result;
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

    LogicalDevice createDevice(const DeviceCreateInfo& createInfo)
    {
        VkDevice deviceHandle;
        call(vkCreateDevice, m_handle, &createInfo, nullptr, &deviceHandle);
        return LogicalDevice(deviceHandle);
    }

    template <typename F>
    std::vector<std::pair<uint32_t, VkQueueFamilyProperties>> findQueueFamily(F&& filter) const
    {
        std::vector<std::pair<uint32_t, VkQueueFamilyProperties>> result;

        uint32_t i = 0;
        for (auto& family : queueFamilies()) {
            if (filter(family)) result.emplace_back(i, family);
            ++i;
        }

        return result;
    }
};
}
