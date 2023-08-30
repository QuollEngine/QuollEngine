#pragma once

#include "quoll/asset/MeshAsset.h"

namespace quoll {

/**
 * @brief Mesh render utilities
 */
class MeshRenderUtils {
  static constexpr size_t SkinGeometryContributors = 3;

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
  static std::array<uint64_t, 1>
  getGeometryBufferOffsets(const MeshAsset &mesh);

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
  static std::array<uint64_t, SkinGeometryContributors>
  getSkinnedGeometryBufferOffsets(const MeshAsset &mesh);
};

} // namespace quoll
