#pragma once

#include "GLTFImportData.h"

namespace quoll::editor {

struct TransformData {
  glm::vec3 localPosition{0.0f};

  glm::quat localRotation{1.0f, 0.0f, 0.0f, 0.0f};

  glm::vec3 localScale{1.0f};

  glm::mat4 localTransform{1.0f};
};

void decomposeMatrix(const glm::mat4 &matrix, glm::vec3 &position,
                     glm::quat &rotation, glm::vec3 &scale);

TransformData loadTransformData(const tinygltf::Node &node);

} // namespace quoll::editor
