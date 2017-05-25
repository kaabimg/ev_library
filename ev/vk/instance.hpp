#pragma once

#include <ev/vk/application.hpp>
#include <ev/vk/device.hpp>

namespace Vk {

struct InstanceCreateInfo : VkInstanceCreateInfo {
    InstanceCreateInfo(const ApplicationInfo& appInfo);

    void setFlags(VkInstanceCreateFlags f);

    void setEnabledLayers(const char* const* names, uint32_t count);

    void setEnabledExtenstions(const char* const* names, uint32_t count);
};

class Instance : public HandleWrapper<VkInstance> {
public:
    Instance();
    Instance(const InstanceCreateInfo& createInfo);
    ~Instance();

    Instance(Instance&& rhs) = default;
    Instance& operator=(Instance&& rhs) = default;

    uint32_t physicalDeviceCount() const;

    std::vector<PhysicalDevice> physicalDevices() const;

    template <typename F>
    std::vector<PhysicalDevice> findPhysicalDevices(F&& f);
};

template <typename F>
inline std::vector<PhysicalDevice> Instance::findPhysicalDevices(F&& f)
{
    std::vector<PhysicalDevice> result;

    for (auto& device : physicalDevices()) {
        if (f(device)) {
            result.push_back(std::move(device));
        }
    }
    return result;
}

}
