#include "liquid/core/Base.h"
#include "EditorRendererFrameData.h"

namespace liquidator {

EditorRendererFrameData::EditorRendererFrameData(
    liquid::rhi::RenderDevice *device, size_t reservedSpace)
    : mReservedSpace(reservedSpace), mDevice(device) {
  mSkeletonTransforms.reserve(mReservedSpace);
  mNumBones.reserve(mReservedSpace);
  mGizmoTransforms.reserve(reservedSpace);
  mSkeletonVector.reset(new glm::mat4[mReservedSpace * MaxNumBones]);

  mCameraBuffer = mDevice->createBuffer(
      {liquid::rhi::BufferType::Uniform, sizeof(liquid::CameraComponent)});

  mEditorGridBuffer = mDevice->createBuffer(
      {liquid::rhi::BufferType::Uniform, sizeof(EditorGridData)});

  mSkeletonTransformsBuffer = mDevice->createBuffer({
      liquid::rhi::BufferType::Storage,
      mReservedSpace * sizeof(glm::mat4),
  });

  mSkeletonBoneTransformsBuffer = mDevice->createBuffer({
      liquid::rhi::BufferType::Storage,
      mReservedSpace * MaxNumBones * sizeof(glm::mat4),
  });

  mGizmoTransformsBuffer = mDevice->createBuffer(
      {liquid::rhi::BufferType::Storage, sizeof(glm::mat4) * mReservedSpace,
       mGizmoTransforms.data()});

  mCollidableEntityBuffer = mDevice->createBuffer(
      {liquid::rhi::BufferType::Uniform, sizeof(CollidableEntity)});
}

void EditorRendererFrameData::addSkeleton(
    const glm::mat4 &worldTransform,
    const std::vector<glm::mat4> &boneTransforms) {
  mSkeletonTransforms.push_back(worldTransform);
  mNumBones.push_back(static_cast<uint32_t>(boneTransforms.size()));

  auto *currentSkeleton = mSkeletonVector.get() + (mLastSkeleton * MaxNumBones);
  size_t dataSize = std::min(boneTransforms.size(), MaxNumBones);
  memcpy(currentSkeleton, boneTransforms.data(), dataSize * sizeof(glm::mat4));

  mLastSkeleton++;
}

void EditorRendererFrameData::setActiveCamera(
    const liquid::CameraComponent &camera) {
  mCameraData = camera;
}

void EditorRendererFrameData::addGizmo(liquid::rhi::TextureHandle icon,
                                       const glm::mat4 &worldTransform) {
  mGizmoTransforms.push_back(worldTransform);
  mGizmoCounts[icon]++;
}

void EditorRendererFrameData::setEditorGrid(const EditorGridData &data) {
  mEditorGridData = data;
}

void EditorRendererFrameData::updateBuffers() {

  mCameraBuffer.update(&mCameraData, sizeof(liquid::CameraComponent));
  mEditorGridBuffer.update(&mEditorGridData, sizeof(EditorGridData));

  if (!mSkeletonTransforms.empty()) {
    mSkeletonTransformsBuffer.update(mSkeletonTransforms.data(),
                                     mSkeletonTransforms.size() *
                                         sizeof(glm::mat4));
    mSkeletonBoneTransformsBuffer.update(
        mSkeletonVector.get(), mLastSkeleton * MaxNumBones * sizeof(glm::mat4));
  }

  mGizmoTransformsBuffer.update(mGizmoTransforms.data(),
                                mGizmoTransforms.size() * sizeof(glm::mat4));

  mCollidableEntityBuffer.update(&mCollidableEntityParams,
                                 sizeof(CollidableEntity));
}

void EditorRendererFrameData::clear() {
  mSkeletonTransforms.clear();
  mGizmoTransforms.clear();
  mNumBones.clear();
  mGizmoCounts.clear();
  mLastSkeleton = 0;

  mCollidableEntity = liquid::EntityNull;
}

void EditorRendererFrameData::setCollidable(
    liquid::Entity entity, const liquid::CollidableComponent &collidable,
    const liquid::WorldTransformComponent &worldTransform) {
  mCollidableEntity = entity;
  mCollidableEntityParams.worldTransform = worldTransform.worldTransform;
  mCollidableEntityParams.type.x =
      static_cast<uint32_t>(collidable.geometryDesc.type);

  if (collidable.geometryDesc.type == liquid::PhysicsGeometryType::Box) {
    const auto &params =
        std::get<liquid::PhysicsGeometryBox>(collidable.geometryDesc.params);
    mCollidableEntityParams.params = glm::vec4(params.halfExtents, 0.0f);
  } else if (collidable.geometryDesc.type ==
             liquid::PhysicsGeometryType::Sphere) {
    const auto &params =
        std::get<liquid::PhysicsGeometrySphere>(collidable.geometryDesc.params);
    mCollidableEntityParams.params = glm::vec4(params.radius);
  } else if (collidable.geometryDesc.type ==
             liquid::PhysicsGeometryType::Capsule) {
    const auto &params = std::get<liquid::PhysicsGeometryCapsule>(
        collidable.geometryDesc.params);
    mCollidableEntityParams.params =
        glm::vec4(params.radius, params.halfHeight, 0.0f, 0.0f);
  }
}

} // namespace liquidator
