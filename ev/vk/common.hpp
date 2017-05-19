#pragma once

#include <vulkan/vulkan.h>
#include <ev/core/basic_types.hpp>

#include <stdexcept>

namespace Vk {

template <typename H>
class HandleWrapper : ev::non_copyable_t {
public:
    using Super = HandleWrapper<H>;

    HandleWrapper()
    {
    }
    HandleWrapper(H h) : m_handle(h)
    {
    }

    HandleWrapper(HandleWrapper&& rhs)
    {
        std::swap(m_handle, rhs.m_handle);
    }

    HandleWrapper& operator=(HandleWrapper&& rhs)
    {
        std::swap(m_handle, rhs.m_handle);
        return *this;
    }

    H handle() const
    {
        return m_handle;
    }

    operator bool() const
    {
        return m_handle != nullptr;
    }

protected:
    H m_handle = nullptr;
};

struct Version {
    Version(uint32_t val) : m_value(val)
    {
    }
    Version(uint32_t maj, uint32_t min, uint32_t patch = 0)
        : m_value(VK_MAKE_VERSION(maj, min, patch))
    {
    }

    uint32_t majorVersion() const
    {
        return VK_VERSION_MAJOR(m_value);
    }

    uint32_t minorVersion() const
    {
        return VK_VERSION_MINOR(m_value);
    }

    uint32_t patchVersion() const
    {
        return VK_VERSION_PATCH(m_value);
    }

    operator uint32_t() const
    {
        return m_value;
    }

private:
    uint32_t m_value = 0;
};

inline const char* errorName(VkResult error)
{
    switch (error) {
        case VK_NOT_READY: return "VK_NOT_READY";
        case VK_TIMEOUT: return "VK_TIMEOUT";
        case VK_EVENT_SET: return "VK_EVENT_SET";
        case VK_EVENT_RESET: return "VK_EVENT_RESET";
        case VK_INCOMPLETE: return "VK_INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case VK_ERROR_VALIDATION_FAILED_EXT: return "VK_ERROR_VALIDATION_FAILED_EXT";
        case VK_ERROR_INVALID_SHADER_NV: return "VK_ERROR_INVALID_SHADER_NV";
        default: return "UNKNOWN ERROR";
    }
}

template <typename F, typename... Args>
inline void call(F&& f, Args&&... args)
{
    VkResult status = f(std::forward<Args>(args)...);
    if (status != VK_SUCCESS) {
        throw std::runtime_error(errorName(status));
    }
}

inline uint32_t availableExtensionCount()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    return extensionCount;
}

std::vector<VkExtensionProperties> availableExtensionProperties()
{
    uint32_t count = availableExtensionCount();
    std::vector<VkExtensionProperties> result(count);
    vkEnumerateInstanceExtensionProperties(nullptr, &count, result.data());
    return result;
}
}
