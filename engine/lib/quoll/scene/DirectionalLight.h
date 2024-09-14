#pragma once

namespace quoll {

struct DirectionalLight {
  glm::vec4 color{1.0f};

  f32 intensity = 1.0f;

  glm::vec3 direction{0.0f};
};

} // namespace quoll
