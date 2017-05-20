#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <ev/vk/application.hpp>
#include <ev/vk/instance.hpp>

#include <ev/core/logging.hpp>

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensionNames = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    ev::debug() << "Extension count" << glfwExtensionCount << "Available"
                << Vk::availableExtensionCount();
    for (uint32_t i = 0; i < glfwExtensionCount; ++i) {
        ev::debug() << "Extension" << i << glfwExtensionNames[i];
    }

    ev::debug() << "Available extensions";

    for (const VkExtensionProperties& extension : Vk::availableExtensionProperties()) {
        ev::debug() << extension.extensionName << extension.specVersion;
    }

    auto window = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);

    ////
    Vk::ApplicationInfo appInfo;
    appInfo.setApplicationName("Test vulkan");
    appInfo.setEngineName("Test engine");
    appInfo.setVersion(Vk::Version(1, 0));
    appInfo.setEngineVersion(Vk::Version(1, 0));
    appInfo.setApiVersion(VK_API_VERSION_1_0);

    Vk::InstanceCreateInfo instanceCreateInfo{appInfo};
    instanceCreateInfo.setEnabledExtenstions(glfwExtensionNames, glfwExtensionCount);

    Vk::Instance instance{instanceCreateInfo};

    for (const Vk::PhysicalDevice& device : instance.physiclDevices()) {
        auto properties = device.properties();
        ev::debug() << "Device" << properties.deviceName;
        ev::debug() << "    Type" << std::to_string(properties.deviceType);

        for(const VkQueueFamilyProperties & qp : device.queueFamilies())
        {
            ev::debug() << "    Queue count" << qp.queueCount;
            ev::debug() << "    Queue flags" << qp.queueFlags;
        }
    }

    //// Main loop
    //    while (!glfwWindowShouldClose(window)) {
    //        glfwPollEvents();
    //    }

    //// Clean
    glfwDestroyWindow(window);
    glfwTerminate();
}
