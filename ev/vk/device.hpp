#pragma once

#include <ev/vk/common.hpp>

namespace Vk {

struct PhysicalDeviceFeatures : VkPhysicalDeviceFeatures {
    PhysicalDeviceFeatures();
};

struct DeviceQueueCreateInfo : VkDeviceQueueCreateInfo {
    DeviceQueueCreateInfo();
    void setQueuesPriorities(std::vector<float> priorities);

private:
    std::vector<float> m_queuesPriorities;
};

struct DeviceCreateInfo : VkDeviceCreateInfo {
    DeviceCreateInfo();

    void setQueueCreateInfos(std::vector<DeviceQueueCreateInfo> queueCreateInfos);
    void setEnabledLayers(const char* const* enabledLayers, uint32_t count);
    PhysicalDeviceFeatures& enabledFeatures();

private:
    PhysicalDeviceFeatures m_features;
    std::vector<DeviceQueueCreateInfo> m_queueCreateInfos;
};

class LogicalDevice : public HandleWrapper<VkDevice> {
public:
    LogicalDevice(VkDevice device);
    ~LogicalDevice();

    LogicalDevice(LogicalDevice&& rhs) = default;
    LogicalDevice& operator=(LogicalDevice&& rhs) = default;
};

class PhysicalDevice : public HandleWrapper<VkPhysicalDevice> {
public:
    PhysicalDevice(VkPhysicalDevice device);

    PhysicalDevice(PhysicalDevice&&) = default;
    PhysicalDevice& operator=(PhysicalDevice&&) = default;

    VkPhysicalDeviceProperties properties() const;

    VkPhysicalDeviceFeatures features() const;

    VkPhysicalDeviceMemoryProperties memoryProperties() const;

    uint32_t queueFamilyCount() const;
    std::vector<VkQueueFamilyProperties> queueFamilies() const;

    LogicalDevice createDevice(const DeviceCreateInfo& createInfo);

    template <typename F>
    std::vector<std::pair<uint32_t, VkQueueFamilyProperties>> findQueueFamilies(F&& filter) const;
};

template <typename F>
inline std::vector<std::pair<uint32_t, VkQueueFamilyProperties>> PhysicalDevice::findQueueFamilies(
    F&& filter) const
{
    std::vector<std::pair<uint32_t, VkQueueFamilyProperties>> result;

    uint32_t i = 0;
    for (auto& family : queueFamilies()) {
        if (filter(family)) result.emplace_back(i, family);
        ++i;
    }

    return result;
}
}
