#pragma once

#include <ev/vk/common.hpp>

namespace Vk {


struct DeviceQueueCreateInfo : VkDeviceQueueCreateInfo {

    DeviceQueueCreateInfo()
    {
        sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        pNext = nullptr;
        flags = 0;
        queueFamilyIndex = 0;
        queueCount = 0;
        pQueuePriorities = 0;
    }
};


class Queue : HandleWrapper<VkQueue>
{
public:
    Queue(VkQueue handle):Super(handle){}



};

}
