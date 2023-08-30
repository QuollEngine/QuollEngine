#pragma once

#include "GLTFImportData.h"

namespace quoll::editor {

/**
 * @brief Transient transform data
 *
 * Stores transform information
 * for all nodes in a GLTF file
 */
struct TransformData {
  /**
   * Local position
   */
  glm::vec3 localPosition{0.0f};

  /**
   * Local rotation
   */
  glm::quat localRotation{1.0f, 0.0f, 0.0f, 0.0f};

  /**
   * Local scale
   */
  glm::vec3 localScale{1.0f};

  /**
   * Local transform matrix
   */
  glm::mat4 localTransform{1.0f};
};

/**
 * @brief Decomposes matrix into TRS values
 *
 * @param matrix Input matrix
 * @param position Output position
 * @param rotation Output rotation
 * @param scale Output scale
 */
void decomposeMatrix(const glm::mat4 &matrix, glm::vec3 &position,
                     glm::quat &rotation, glm::vec3 &scale);
/**
 * @brief Load transform data
 *
 * Load matrix or each transform
 * attribute based on the given
 * ones
 *
 * @param node TinyGLTF node
 * @return Transform data
 */
TransformData loadTransformData(const tinygltf::Node &node);

} // namespace quoll::editor
