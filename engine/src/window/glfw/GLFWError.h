#pragma once

namespace liquid {

/**
 * @brief GLFW Error
 *
 * This exception should be used for all
 * GLFW related fatal errors
 */
class GLFWError : public std::runtime_error {
public:
  GLFWError(const String &what) : std::runtime_error("[GLFWError] " + what) {}
};

} // namespace liquid
