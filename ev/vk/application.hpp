#pragma once

#include <ev/vk/common.hpp>

#include <string>

namespace Vk {

struct ApplicationInfo : VkApplicationInfo {
    ApplicationInfo()
    {
        sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        pNext = nullptr;
        pApplicationName = "";
        applicationVersion = makeVersion(1, 0);
        pEngineName = "";
        engineVersion = makeVersion(1, 0);
        apiVersion = makeVersion(1, 0);
    }

    void setApplicationName(const std::string& name)
    {
        m_appName = name;
        pApplicationName = m_appName.c_str();
    }



private:
    std::string m_appName, m_engineName;
};
}
