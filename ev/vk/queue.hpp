#pragma once

#include <ev/vk/common.hpp>

namespace Vk {


class Queue : public HandleWrapper<VkQueue> {
public:
    Queue(VkQueue handle) : WrapperType(handle)
    {
    }
};
}
