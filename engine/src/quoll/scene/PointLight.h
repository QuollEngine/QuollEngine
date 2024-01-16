#pragma once

namespace quoll {

struct PointLight {
  glm::vec4 color{1.0};

  f32 intensity = 1.0f;

  f32 range = 10.0f;
};

} // namespace quoll
