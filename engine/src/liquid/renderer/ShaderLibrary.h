#pragma once

#include "liquid/rhi/RenderHandle.h"

namespace liquid {

/**
 * @brief Shader library
 */
class ShaderLibrary {
public:
  /**
   * @brief Add shader
   *
   * @param name Shader name
   * @param shader Shader object
   */
  void addShader(StringView name, rhi::ShaderHandle shader);

  /**
   * @brief Get shader
   *
   * @param name Shader name
   * @return Shader
   */
  rhi::ShaderHandle getShader(StringView name) const;

private:
  std::unordered_map<String, rhi::ShaderHandle> mShaders;
};

} // namespace liquid
