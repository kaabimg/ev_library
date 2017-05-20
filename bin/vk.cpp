#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <ev/vk/application.hpp>
#include <ev/vk/instance.hpp>

#include <ev/core/logging_helpers.hpp>

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
        ev::debug() << "    QFCt" << device.queueFamilyCount();

        //        for (const VkQueueFamilyProperties& qp : device.queueFamilies()) {
        for (auto & [ index, qp ] :
             device.findQueueFamily([](const VkQueueFamilyProperties& f) { return true; })) {
            ev::debug() << "    *** Queue" << index;
            ev::debug() << "        Count" << qp.queueCount;
            ev::debug() << "        Flags" << qp.queueFlags;
            ev::debug() << "        Graph" << ((qp.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0);
            ev::debug() << "        Compu" << ((qp.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0);
            ev::debug() << "        Trans" << ((qp.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0);
            ev::debug() << "        Sp.B." << ((qp.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0);
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
