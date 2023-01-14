#include "liquid/core/Base.h"
// TODO: Remove engine logger

#include "liquid/core/Engine.h"
#include "TransformUtils.h"

namespace liquidator {

void decomposeMatrix(const glm::mat4 &matrix, glm::vec3 &position,
                     glm::quat &rotation, glm::vec3 &scale) {
  glm::mat4 temp = matrix;
  position = temp[3];

  for (glm::mat4::length_type i = 0; i < 3; ++i) {
    scale[i] = glm::length(temp[i]);
    temp[i] /= scale[i];
  }

  rotation = glm::toQuat(matrix);
}

TransformData loadTransformData(const tinygltf::Node &node) {
  TransformData data{};

  static constexpr size_t TransformMatrixSize = 6;

  glm::mat4 finalTransform = glm::mat4{1.0f};
  if (node.matrix.size() == TransformMatrixSize) {
    finalTransform = glm::make_mat4(node.matrix.data());
    decomposeMatrix(finalTransform, data.localPosition, data.localRotation,
                    data.localScale);

  } else if (node.matrix.size() > 0) {
    liquid::Engine::getLogger().warning()
        << "Node matrix data must have 16 values. Skipping...";
  } else {
    if (node.translation.size() == glm::vec3::length()) {
      data.localPosition = glm::make_vec3(node.translation.data());
      finalTransform *= glm::translate(glm::mat4{1.0f}, data.localPosition);
    } else if (node.translation.size() > 0) {
      liquid::Engine::getLogger().warning()
          << "Node translation data must have 3 values. Skipping...";
    }

    if (node.rotation.size() == glm::quat::length()) {
      data.localRotation = glm::make_quat(node.rotation.data());
      finalTransform *= glm::toMat4(data.localRotation);
    } else if (node.rotation.size() > 0) {
      liquid::Engine::getLogger().warning()
          << "Node rotation data must have 4 values. Skipping...";
    }

    if (node.scale.size() == glm::vec3::length()) {
      data.localScale = glm::make_vec3(node.scale.data());
      finalTransform *= glm::scale(glm::mat4{1.0f}, data.localScale);
    } else if (node.scale.size() > 0) {
      liquid::Engine::getLogger().warning()
          << "Node scale data must have 3 values. Skipping...";
    }
  }

  data.localTransform = finalTransform;

  return data;
}

} // namespace liquidator
