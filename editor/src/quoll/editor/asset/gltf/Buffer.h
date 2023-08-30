#pragma once

namespace quoll::editor {

/**
 * @brief Transient buffer meta data
 *
 * Stores accessor, bufferview, and raw data
 * for easy operations
 */
struct BufferMeta {
  /**
   * TinyGLTF accessor
   */
  tinygltf::Accessor accessor;

  /**
   * TinyGLTF buffer view
   */
  tinygltf::BufferView bufferView;

  /**
   * Raw data
   */
  const unsigned char *rawData;
};

/**
 * @brief Geta buffer metadata for accessor
 *
 * @param model TinyGLTF model
 * @param accessorIndex Index of buffer accessor
 * @return GLTF buffer metadata
 */
BufferMeta getBufferMetaForAccessor(const tinygltf::Model &model,
                                    int accessorIndex);

} // namespace quoll::editor
