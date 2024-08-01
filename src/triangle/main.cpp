
#include <cstring>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class HelloTriangleApplication {
public:
  void run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

private:
  GLFWwindow *window;
  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;

  void initWindow() {
    glfwInit();
    // tell glfw to not use opengl but user specified api
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // tell glfw to disallow window resizing, will be handled later in the
    // tutorial
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
  }

  void createInstance() {
    // check for misconfig
    if (enableValidationLayers && !checkValidationLayerSupport()) {
      throw std::runtime_error(
          "validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // In Vulkan a lot of information is passed through structs like below
    // createInfo tells Vulkan which global extension and validation layer we
    // want to use
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // Add validation layers if instructed to do so
    if (enableValidationLayers) {
      createInfo.enabledLayerCount =
          static_cast<uint32_t>(validationLayers.size());
      createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
      createInfo.enabledLayerCount = 0;
    }

    // Vulkan needs extension to interface with window system, glfw tells which
    // extension needs to be done
    // adds callback for validationlayer if enabled to the required extensions
    // which then get added
    auto extensions = getRequiredExtension();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    createInfo.enabledLayerCount = 0;
    // create the vulkan instance with above settings applied
    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
      throw std::runtime_error("failed to create instance!");
    }
  }

  bool required_supported(const char **&glfwExtensions,
                          const uint32_t glfwExtensionCount) {
    /*
    Function for checking if returned required Extensions by glfw are supported
    by the system. (It think its redundant but it was a neat little task to
    solve)
     */
    // retrieve list of supported extensions
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    // array for holding extension info
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                           extensions.data());

    bool contains = true;

    for (size_t i = 0; i < glfwExtensionCount; ++i) {
      for (const auto &extension : extensions) {
        contains ^= extension.extensionName == glfwExtensions[i];
      }
      if (!contains) {
        return false;
      }
    }

    return contains;
  }

  bool checkValidationLayerSupport() {
    /*
    Function for checking if validation layers are supported by the system
     */
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName : validationLayers) {
      bool layerFound = false;

      for (const auto &layerProperties : availableLayers) {
        if (strcmp(layerName, layerProperties.layerName) == 0) {
          layerFound = true;
          break;
        }
      }
      if (!layerFound) {
        return false;
      }
    }
    return true;
  }

  std::vector<const char *> getRequiredExtension() {
    /*
    Function for creating a callback to handle messages for the validation
    layer.
    */
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions,
                                         glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
      extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    return extensions;
  }

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCAllback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData) {
    /**
     *   Debug Callback function.
     *
     * @param messageSeverity Flags for Message Severity:
     *       _VERBOSE_: Diagnostic message
     *       _INFO_: Informational message like the creation of a resource
     *       _WARNING_: Message about behaviour that is not necessarily an
     * error, but very likely a bug in your application
     *       _ERROR_: Message about
     * behaviour that is invalid and may cause crashes
     * @param messageType has the following options:
     *       _GENERAL_: Some event has happened that is unrelated to the
     *specification or performance _VALIDATION: Something has happened that
     *violates the specification or indicates a possible mistake _PERFORMANCE:
     *Potential non-optimal use of Vulkan
     * @param pCallbackData contains the details of message itself:
     *       pMessage: The debug message as a null-terminated string
     *       pObjects: Array of Vulkan object handles related to the message
     *       objectCount: Number of objects in array
     * @param pUserData contains a pointer to pass user own data for use
     **/
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
      // Message important enough to show
    }
    return VK_FALSE;
  }

  void initVulkan() {
    createInstance();
    setupDebugMessenger();
  }

  void setupDebugMessenger() {
    if (!enableValidationLayers)
      return;
  }
  void mainLoop() {
    // repeats loop for displaying stuff in window until condition for window
    // closing is met
    //  like closing the window for example
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();
    }
  }

  void cleanup() {
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);

    glfwTerminate();
  }
};

int main() {
  HelloTriangleApplication app;

  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}