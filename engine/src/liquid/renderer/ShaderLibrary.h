#pragma once

#include "Shader.h"

namespace liquid {

class ShaderLibrary {
public:
  /**
   * @brief Add shader
   *
   * @param name Shader name
   * @param shader Shader object
   */
  void addShader(const String &name, const SharedPtr<Shader> &shader);

  /**
   * @brief Get shader
   *
   * @param name Shader name
   * @return Shader
   */
  const SharedPtr<Shader> &getShader(const String &name) const;

private:
  std::unordered_map<String, SharedPtr<Shader>> shaders;
};

} // namespace liquid
