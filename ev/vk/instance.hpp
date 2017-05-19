#pragma once

#include <ev/vk/application.hpp>
#include <ev/vk/device.hpp>

namespace Vk {

struct InstanceCreateInfo : VkInstanceCreateInfo {
    InstanceCreateInfo(const ApplicationInfo& appInfo)
    {
        sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        pNext = nullptr;
        pApplicationInfo = &appInfo;

        flags = 0;
        enabledLayerCount = 0;
        ppEnabledLayerNames = nullptr;
        enabledExtensionCount = 0;
        ppEnabledExtensionNames = nullptr;
    }

    void setFlags(VkInstanceCreateFlags f)
    {
        flags = f;
    }

    void setEnabledLayers(const char* const* names, uint32_t count)
    {
        ppEnabledLayerNames = names;
        enabledLayerCount = count;
    }

    void setEnabledExtenstions(const char* const* names, uint32_t count)
    {
        ppEnabledExtensionNames = names;
        enabledExtensionCount = count;
    }
};

class Instance : HandleWrapper<VkInstance> {
public:
    Instance()
    {
    }
    Instance(const InstanceCreateInfo& createInfo)
    {
        call(vkCreateInstance, &createInfo, nullptr, &m_handle);
    }
    ~Instance()
    {
        if (m_handle) vkDestroyInstance(m_handle, nullptr);
    }

    Instance(Instance&& rhs) = default;
    Instance& operator=(Instance&& rhs) = default;

    uint32_t physicalDeviceCount() const
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_handle, &deviceCount, nullptr);
        return deviceCount;
    }

    std::vector<VkPhysicalDevice> physiclDevices() const
    {
        uint32_t deviceCount = physicalDeviceCount();
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_handle, &deviceCount, devices.data());
        return devices;
    }

};
}
