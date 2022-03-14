#pragma once

#include "liquid/rhi/RenderHandle.h"

namespace liquid {

class ShaderLibrary {
public:
  /**
   * @brief Add shader
   *
   * @param name Shader name
   * @param shader Shader object
   */
  void addShader(const String &name, ShaderHandle shader);

  /**
   * @brief Get shader
   *
   * @param name Shader name
   * @return Shader
   */
  ShaderHandle getShader(const String &name) const;

private:
  std::unordered_map<String, ShaderHandle> shaders;
};

} // namespace liquid
