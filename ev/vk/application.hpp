#pragma once

#include <ev/vk/common.hpp>

#include <string>

namespace Vk {

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
}
