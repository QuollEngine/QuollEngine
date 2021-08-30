#pragma once

namespace liquid {

class Shader {
public:
  /**
   * @brief Destroy shader
   */
  virtual ~Shader() = default;

  Shader() = default;
  Shader(const Shader &rhs) = delete;
  Shader(Shader &&rhs) = delete;
  Shader &operator=(const Shader &rhs) = delete;
  Shader &operator=(Shader &&rhs) = delete;
};

} // namespace liquid
