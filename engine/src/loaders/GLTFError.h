#pragma once

namespace liquid {

/**
 * @brief GLTF Error
 *
 * This exception should be used for all
 * GLTF loader related fatal errors
 */
class GLTFError : public std::runtime_error {
public:
  GLTFError(const String &what) : std::runtime_error("[GLTF] " + what){};
};

} // namespace liquid
