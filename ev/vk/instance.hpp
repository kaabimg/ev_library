#pragma once

#include <ev/vk/application.hpp>

namespace Vk {

struct InstanceCreateInfo : VkInstanceCreateInfo {

    InstanceCreateInfo(const ApplicationInfo& appInfo)
    {
        sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        pNext = nullptr;
        pApplicationInfo = &appInfo;
    }
};


struct Instance {

    Instance (const InstanceCreateInfo & createInfo)
    {
        call(vkCreateInstance,&createInfo,nullptr,&m_data);
    }

private:
    VkInstance m_data = nullptr;
};


}
