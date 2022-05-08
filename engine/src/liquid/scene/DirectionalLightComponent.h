#pragma once

namespace liquid {

struct DirectionalLightComponent {
  glm::vec4 color{1.0f};
  float intensity = 1.0f;
  glm::vec3 direction{0.0f};
};

} // namespace liquid
