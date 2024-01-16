#include "quoll/core/Base.h"
#include "TransformUtils.h"

namespace quoll::editor {

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

  static constexpr usize TransformMatrixSize = 6;

  glm::mat4 finalTransform = glm::mat4{1.0f};
  if (node.matrix.size() == TransformMatrixSize) {
    finalTransform = glm::make_mat4(node.matrix.data());
    decomposeMatrix(finalTransform, data.localPosition, data.localRotation,
                    data.localScale);

  } else {
    if (node.translation.size() == glm::vec3::length()) {
      data.localPosition = glm::make_vec3(node.translation.data());
      finalTransform *= glm::translate(glm::mat4{1.0f}, data.localPosition);
    }

    if (node.rotation.size() == glm::quat::length()) {
      data.localRotation = glm::make_quat(node.rotation.data());
      finalTransform *= glm::toMat4(data.localRotation);
    }

    if (node.scale.size() == glm::vec3::length()) {
      data.localScale = glm::make_vec3(node.scale.data());
      finalTransform *= glm::scale(glm::mat4{1.0f}, data.localScale);
    }
  }

  data.localTransform = finalTransform;

  return data;
}

} // namespace quoll::editor
