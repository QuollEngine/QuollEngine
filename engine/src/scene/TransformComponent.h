#pragma once

namespace liquid {

struct TransformComponent {
  glm::mat4 transformLocal;
  glm::mat4 transformWorld{1.0f};
};

} // namespace liquid
