#include "liquid/core/Base.h"
#include "VulkanHeaders.h"
#include "VulkanError.h"

// Reference:
// https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkResult.html
static const std::map<VkResult, quoll::String> resultMap{
    // Success codes
    {VK_SUCCESS, "Operation successfully completed"},
    {VK_NOT_READY, "Operation has not yet completed"},
    {VK_TIMEOUT, "Operation timed out"},
    {VK_EVENT_SET, "Event is signaled"},
    {VK_EVENT_RESET, "Event is unsignaled"},
    {VK_INCOMPLETE, "Result incomplete"},
    {VK_THREAD_IDLE_KHR, "Thread is idle"},
    {VK_THREAD_DONE_KHR, "Thread is done"},
    {VK_OPERATION_DEFERRED_KHR, "Some operations were deferred"},
    {VK_OPERATION_NOT_DEFERRED_KHR, "No operations were deferred."},
    {VK_SUBOPTIMAL_KHR,
     "Swapchain no longer matches surface properties exactly"},

    // Extension success codes
    {VK_PIPELINE_COMPILE_REQUIRED_EXT,
     "A requested pipeline creation would have required compilation, but the "
     "application requested compilation to not be performed."},

    // Error codes
    {VK_ERROR_INITIALIZATION_FAILED, "Initialization failed"},
    {VK_ERROR_DEVICE_LOST, "Device lost"},
    {VK_ERROR_LAYER_NOT_PRESENT, "Requested layer is not present"},
    {VK_ERROR_EXTENSION_NOT_PRESENT, "Requested extension is not supported"},
    {VK_ERROR_FEATURE_NOT_PRESENT, "Requested feature is not supported"},
    {VK_ERROR_INCOMPATIBLE_DRIVER,
     "Request Vulkan version is not supported by driver"},
    {VK_ERROR_FORMAT_NOT_SUPPORTED, "Requested format is not supported"},

    {VK_ERROR_OUT_OF_HOST_MEMORY, "Out of host memory"},
    {VK_ERROR_OUT_OF_DEVICE_MEMORY, "Out of device memory"},
    {VK_ERROR_MEMORY_MAP_FAILED, "Memory map failed"},
    {VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS,
     "Requested address is not available"},
    {VK_ERROR_FRAGMENTED_POOL, "Pool memory is fragmented"},
    {VK_ERROR_OUT_OF_POOL_MEMORY, "Out of pool memory"},
    {VK_ERROR_FRAGMENTATION, "Pool memory is fragmented"},
    {VK_ERROR_TOO_MANY_OBJECTS, "Too many objects are been created"},

    {VK_ERROR_INVALID_SHADER_NV, "Invalid shader"},
    {VK_ERROR_INVALID_EXTERNAL_HANDLE, "External handle is not valid"},
    {VK_ERROR_UNKNOWN, "Unknown error"},

    {VK_ERROR_SURFACE_LOST_KHR, "Surface is no longer available"},
    {VK_ERROR_NATIVE_WINDOW_IN_USE_KHR, "Requested window is already in use"},
    {VK_ERROR_OUT_OF_DATE_KHR,
     "Surface is no longer compatible with existing swapchain"},
    {VK_ERROR_INCOMPATIBLE_DISPLAY_KHR,
     "Display used uses different image layout than swapchain"},

    // Extension errors
    {VK_ERROR_VALIDATION_FAILED_EXT, "Validation failed"},
    {VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT,
     "Invalid DRM format modifier plane layout"},
    {VK_ERROR_NOT_PERMITTED_EXT, "Operation not permitted"},
    {VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT,
     "Swapchain does not have exclusive full screen access"},
};

namespace quoll::rhi {

String createVulkanErrorMessage(VkResult resultCode, const String &what,
                                const String &debugName) {
  String errorMessage = "[VulkanError] " + what;
  if (resultCode == VK_SUCCESS) {
    return errorMessage;
  }

  const auto &codeString = "(code: " + std::to_string(resultCode) + ")";
  const auto &it = resultMap.find(resultCode);
  const auto &humanReadableResultString =
      it != resultMap.end() ? (*it).second : "Unknown Error";

  auto debugNameString = !debugName.empty() ? "Resource: " + debugName : "";

  std::stringstream ss;
  ss << errorMessage << ": " << humanReadableResultString << " " << codeString
     << ". " << debugNameString;

  return ss.str();
}

} // namespace quoll::rhi
