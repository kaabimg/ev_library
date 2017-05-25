#include "instance.hpp"
using namespace Vk;

InstanceCreateInfo::InstanceCreateInfo(const ApplicationInfo &appInfo)
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

void InstanceCreateInfo::setFlags(VkInstanceCreateFlags f)
{
    flags = f;
}

void InstanceCreateInfo::setEnabledLayers(const char * const *names, uint32_t count)
{
    ppEnabledLayerNames = names;
    enabledLayerCount = count;
}

void InstanceCreateInfo::setEnabledExtenstions(const char * const *names, uint32_t count)
{
    ppEnabledExtensionNames = names;
    enabledExtensionCount = count;
}

Instance::Instance() : WrapperType()
{
}

Instance::Instance(const InstanceCreateInfo &createInfo)
{
    call(vkCreateInstance, &createInfo, nullptr, &m_handle);
}

Instance::~Instance()
{
    if (m_handle) vkDestroyInstance(m_handle, nullptr);
}

uint32_t Instance::physicalDeviceCount() const
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_handle, &deviceCount, nullptr);
    return deviceCount;
}

std::vector<PhysicalDevice> Instance::physicalDevices() const
{
    uint32_t deviceCount = physicalDeviceCount();
    std::vector<VkPhysicalDevice> vkDevices(deviceCount);
    vkEnumeratePhysicalDevices(m_handle, &deviceCount, vkDevices.data());

    std::vector<PhysicalDevice> devices;
    devices.reserve(deviceCount);

    for (auto handle : vkDevices) devices.emplace_back(handle);

    return devices;
}
