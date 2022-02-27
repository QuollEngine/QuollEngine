#pragma once

namespace liquid {

struct TransformComponent {
  glm::vec3 localPosition{0.0f};
  glm::quat localRotation{1.0f, 0.0f, 0.0f, 0.0f};
  glm::vec3 localScale{1.0f};

  glm::mat4 worldTransform{1.0f};
  Entity parent = ENTITY_MAX;
};

} // namespace liquid
