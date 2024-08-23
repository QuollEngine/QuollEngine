#pragma once

namespace quoll {

struct CollisionHit {
  glm::vec3 normal;

  f32 distance = 0.0f;

  Entity entity;
};

} // namespace quoll
