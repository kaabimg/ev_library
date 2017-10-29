#pragma once

#include <ev/vk/string_helpers.hpp>
#include <ev/core/basic_types.hpp>

#include <stdexcept>

namespace Vk {

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

struct ApplicationInfo : VkApplicationInfo {
    ApplicationInfo()
    {
        sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        pNext = nullptr;
        pApplicationName = nullptr;
        pEngineName = nullptr;
        applicationVersion = Version(0, 0);
        engineVersion = Version(0, 0);
        apiVersion = Version(0, 0);
    }

    void setApplicationName(const char* name)
    {
        pApplicationName = name;
    }

    void setEngineName(const char* name)
    {
        pEngineName = name;
    }

    void setVersion(Version version)
    {
        applicationVersion = version;
    }

    void setEngineVersion(Version version)
    {
        engineVersion = version;
    }

    void setApiVersion(Version version)
    {
        apiVersion = version;
    }
};

template <typename H>
class HandleWrapper : ev::non_copyable {
public:
    using WrapperType = HandleWrapper<H>;

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

template <typename F, typename... Args>
inline void call(F&& f, Args&&... args)
{
    VkResult status = f(std::forward<Args>(args)...);
    if (status != VK_SUCCESS) {
        throw std::runtime_error(std::to_string(status));
    }
}

inline uint32_t availableExtensionCount()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    return extensionCount;
}

inline std::vector<VkExtensionProperties> availableExtensionProperties()
{
    uint32_t count = availableExtensionCount();
    std::vector<VkExtensionProperties> result(count);
    vkEnumerateInstanceExtensionProperties(nullptr, &count, result.data());
    return result;
}
}
