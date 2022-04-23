#pragma once

namespace liquid {

class RenderStorage {
  static constexpr size_t DEFAULT_RESERVED_SPACE = 10000;
  static constexpr size_t MAX_NUM_JOINTS = 32;

public:
  /**
   * @brief Create render storage
   *
   * @param reservedSpace Reserved space for buffer data
   */
  RenderStorage(size_t reservedSpace = DEFAULT_RESERVED_SPACE);

  /**
   * @brief Update storage buffers
   *
   * @param registry Resource registry
   */
  void updateBuffers(rhi::ResourceRegistry &registry);

  /**
   * @brief Get mesh transforms buffer
   *
   * @return Mesh transforms buffer
   */
  inline rhi::BufferHandle getMeshTransformsBuffer() const {
    return mMeshTransformsBuffer;
  }

  /**
   * @brief Get skinned mesh transforms buffer
   *
   * @return Skinned mesh transforms buffer
   */
  inline rhi::BufferHandle getSkinnedMeshTransformsBuffer() const {
    return mSkinnedMeshTransformsBuffer;
  }

  /**
   * @brief Get skeletons buffer
   *
   * @return Skeletons buffer
   */
  inline rhi::BufferHandle getSkeletonsBuffer() const {
    return mSkeletonsBuffer;
  }

  /**
   * @brief Add mesh data
   *
   * @param transform Mesh world transform
   */
  void addMeshData(const glm::mat4 &transform);

  /**
   * @brief Add skinned mesh data
   *
   * @param transform Skinned mesh world transform
   * @param skeleton Skeleton joint transforms
   */
  void addSkinnedMeshData(const glm::mat4 &transform,
                          const std::vector<glm::mat4> &skeleton);

  /**
   * @brief Clear intermediary buffers
   */
  void clear();

private:
  std::vector<glm::mat4> mMeshTransformMatrices;
  std::vector<glm::mat4> mSkinnedMeshTransformMatrices;
  std::unique_ptr<glm::mat4> mSkeletonVector;
  size_t mLastSkeleton = 0;

  rhi::BufferHandle mMeshTransformsBuffer = rhi::BufferHandle::Invalid;
  rhi::BufferHandle mSkinnedMeshTransformsBuffer = rhi::BufferHandle::Invalid;
  rhi::BufferHandle mSkeletonsBuffer = rhi::BufferHandle::Invalid;

  size_t mReservedSpace = 0;
};

} // namespace liquid
