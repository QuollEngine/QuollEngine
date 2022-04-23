#include "liquid/core/Base.h"

#include "liquid/rhi/RenderHandle.h"
#include "liquid/rhi/ResourceRegistry.h"
#include "RenderStorage.h"

namespace liquid {

RenderStorage::RenderStorage(size_t reservedSpace)
    : mReservedSpace(reservedSpace) {
  mMeshTransformMatrices.reserve(mReservedSpace);
  mSkinnedMeshTransformMatrices.reserve(mReservedSpace);

  mSkeletonVector.reset(new glm::mat4[mReservedSpace * MAX_NUM_JOINTS]);
}

void RenderStorage::updateBuffers(rhi::ResourceRegistry &registry) {
  mMeshTransformsBuffer = registry.setBuffer(
      {
          rhi::BufferType::Storage,
          mReservedSpace * sizeof(glm::mat4),
          mMeshTransformMatrices.data(),
      },
      mMeshTransformsBuffer);

  mSkinnedMeshTransformsBuffer = registry.setBuffer(
      {
          rhi::BufferType::Storage,
          mReservedSpace * sizeof(glm::mat4),
          mSkinnedMeshTransformMatrices.data(),
      },
      mSkinnedMeshTransformsBuffer);

  mSkeletonsBuffer = registry.setBuffer(
      {
          rhi::BufferType::Storage,
          mReservedSpace * MAX_NUM_JOINTS * sizeof(glm::mat4),
          mSkeletonVector.get(),
      },
      mSkeletonsBuffer);
}

void RenderStorage::addMeshData(const glm::mat4 &transform) {
  mMeshTransformMatrices.push_back(transform);
}

void RenderStorage::addSkinnedMeshData(const glm::mat4 &transform,
                                       const std::vector<glm::mat4> &skeleton) {
  mSkinnedMeshTransformMatrices.push_back(transform);

  auto *currentSkeleton =
      mSkeletonVector.get() + (mLastSkeleton * MAX_NUM_JOINTS);
  size_t dataSize = std::min(skeleton.size(), MAX_NUM_JOINTS);
  memcpy(currentSkeleton, skeleton.data(), dataSize * sizeof(glm::mat4));

  mLastSkeleton++;
}

void RenderStorage::clear() {
  mMeshTransformMatrices.clear();
  mSkinnedMeshTransformMatrices.clear();
  mLastSkeleton = 0;
}

} // namespace liquid
