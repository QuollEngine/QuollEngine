#pragma once

namespace quoll {

/**
 * Collision hit data
 */
struct CollisionHit {
  /**
   * Normal of the surface
   */
  glm::vec3 normal;

  /**
   * Collision distance
   */
  f32 distance = 0.0f;
};

} // namespace quoll
