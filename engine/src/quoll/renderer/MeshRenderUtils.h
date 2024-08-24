#pragma once

#include "MeshAsset.h"

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
   *
   * @param mesh Mesh asset data
   * @return Buffers
   */
  static std::array<rhi::BufferHandle, 1>
  getGeometryBuffers(const MeshAsset &mesh);

  /**
   * @brief Get buffer offsets required for mesh geometry
   *
   * @param mesh Mesh asset data
   * @return Offsets
   */
  static std::array<u64, 1> getGeometryBufferOffsets(const MeshAsset &mesh);

  /**
   * @brief Get buffers required for skinned mesh geometry
   *
   * Provides minimal buffer to render
   * the skinned mesh with no shading
   *
   * @param mesh Mesh asset data
   * @return Buffers
   */
  static std::array<rhi::BufferHandle, SkinGeometryContributors>
  getSkinnedGeometryBuffers(const MeshAsset &mesh);

  /**
   * @brief Get buffer offsets required for skinned mesh geometry
   *
   * @param mesh Mesh asset data
   * @return Offsets
   */
  static std::array<u64, SkinGeometryContributors>
  getSkinnedGeometryBufferOffsets(const MeshAsset &mesh);
};

} // namespace quoll
