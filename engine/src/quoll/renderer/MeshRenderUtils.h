#pragma once

#include "MeshDrawData.h"

namespace quoll {

/**
 * Mesh render utilities
 */
class MeshRenderUtils {
  static constexpr usize SkinGeometryContributors = 3;

public:
  /**
   * @brief Get buffers required for mesh geometry
   *
   * Provides minimal buffer to render the mesh
   * with no shading
   */
  static std::array<rhi::BufferHandle, 1>
  getGeometryBuffers(const MeshDrawData *drawData);

  /**
   * @brief Get buffer offsets required for mesh geometry
   */
  static std::array<u64, 1>
  getGeometryBufferOffsets(const MeshDrawData *drawData);

  /**
   * @brief Get buffers required for skinned mesh geometry
   *
   * Provides minimal buffer to render
   * the skinned mesh with no shading
   */
  static std::array<rhi::BufferHandle, SkinGeometryContributors>
  getSkinnedGeometryBuffers(const MeshDrawData *drawData);

  /**
   * @brief Get buffer offsets required for skinned mesh geometry
   */
  static std::array<u64, SkinGeometryContributors>
  getSkinnedGeometryBufferOffsets(const MeshDrawData *drawData);
};

} // namespace quoll
