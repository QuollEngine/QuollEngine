#pragma once

#include "quoll/rhi/RenderHandle.h"

namespace quoll {

struct MeshGeometryInfo {
  u32 numVertices = 0;
  u32 numIndices = 0;
};

struct MeshDrawData {
  std::vector<rhi::BufferHandle> vertexBuffers;
  std::vector<u64> vertexBufferOffsets;
  rhi::BufferHandle indexBuffer = rhi::BufferHandle::Null;

  std::vector<MeshGeometryInfo> geometries;
};

} // namespace quoll
