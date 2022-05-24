#include "liquid/core/Base.h"
#include "EditorRendererStorage.h"

namespace liquidator {

EditorRendererStorage::EditorRendererStorage(size_t reservedSpace)
    : mReservedSpace(reservedSpace) {
  mSkeletonTransforms.reserve(mReservedSpace);
  mNumBones.reserve(mReservedSpace);
  mGizmoTransforms.reserve(reservedSpace);
  mSkeletonVector.reset(new glm::mat4[mReservedSpace * MAX_NUM_BONES]);
}

void EditorRendererStorage::addSkeleton(
    const glm::mat4 &worldTransform,
    const std::vector<glm::mat4> &boneTransforms) {
  mSkeletonTransforms.push_back(worldTransform);
  mNumBones.push_back(static_cast<uint32_t>(boneTransforms.size()));

  auto *currentSkeleton =
      mSkeletonVector.get() + (mLastSkeleton * MAX_NUM_BONES);
  size_t dataSize = std::min(boneTransforms.size(), MAX_NUM_BONES);
  memcpy(currentSkeleton, boneTransforms.data(), dataSize * sizeof(glm::mat4));

  mLastSkeleton++;
}

void EditorRendererStorage::setActiveCamera(
    const liquid::CameraComponent &camera) {
  mCameraData = camera;
}

void EditorRendererStorage::addGizmo(liquid::rhi::TextureHandle icon,
                                     const glm::mat4 &worldTransform) {
  mGizmoTransforms.push_back(worldTransform);
  mGizmoCounts[icon]++;
}

void EditorRendererStorage::setEditorGrid(const EditorGridData &data) {
  mEditorGridData = data;
}

void EditorRendererStorage::updateBuffers(
    liquid::rhi::ResourceRegistry &registry) {

  mCameraBuffer =
      registry.setBuffer({liquid::rhi::BufferType::Uniform,
                          sizeof(liquid::CameraComponent), &mCameraData},
                         mCameraBuffer);

  mEditorGridBuffer =
      registry.setBuffer({liquid::rhi::BufferType::Uniform,
                          sizeof(EditorGridData), &mEditorGridData},
                         mEditorGridBuffer);

  if (!mSkeletonTransforms.empty()) {
    mSkeletonTransformsBuffer = registry.setBuffer(
        {
            liquid::rhi::BufferType::Storage,
            mReservedSpace * sizeof(glm::mat4),
            mSkeletonTransforms.data(),
        },
        mSkeletonTransformsBuffer);

    mSkeletonBoneTransformsBuffer = registry.setBuffer(
        {
            liquid::rhi::BufferType::Storage,
            mReservedSpace * MAX_NUM_BONES * sizeof(glm::mat4),
            mSkeletonVector.get(),
        },
        mSkeletonBoneTransformsBuffer);
  }

  mGizmoTransformsBuffer = registry.setBuffer(
      {liquid::rhi::BufferType::Storage, sizeof(glm::mat4) * mReservedSpace,
       mGizmoTransforms.data()},
      mGizmoTransformsBuffer);
}

void EditorRendererStorage::clear() {
  mSkeletonTransforms.clear();
  mGizmoTransforms.clear();
  mNumBones.clear();
  mGizmoCounts.clear();
  mLastSkeleton = 0;
}

} // namespace liquidator
