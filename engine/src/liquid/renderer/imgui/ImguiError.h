#pragma once

namespace liquid {

/**
 * @brief Imgui Error
 *
 * This exception should be used for all
 * Imgui related fatal errors
 *
 * @deprecated
 */
class ImguiError : public std::runtime_error {
public:
  /**
   * @brief Create imgui error
   *
   * @param what Error message
   */
  ImguiError(const String &what) : std::runtime_error("[Imgui] " + what){};
};

} // namespace liquid
