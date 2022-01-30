#pragma once

namespace liquid {

/**
 * @brief Imgui Error
 *
 * This exception should be used for all
 * Imgui related fatal errors
 */
class ImguiError : public std::runtime_error {
public:
  ImguiError(const String &what) : std::runtime_error("[Imgui] " + what){};
};

} // namespace liquid
