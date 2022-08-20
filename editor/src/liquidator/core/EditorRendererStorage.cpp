#include "liquid/core/Base.h"
#include "EditorRendererStorage.h"

namespace liquidator {

EditorRendererStorage::EditorRendererStorage(liquid::rhi::RenderDevice *device,
                                             size_t reservedSpace)
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

void EditorRendererStorage::addSkeleton(
    const glm::mat4 &worldTransform,
    const std::vector<glm::mat4> &boneTransforms) {
  mSkeletonTransforms.push_back(worldTransform);
  mNumBones.push_back(static_cast<uint32_t>(boneTransforms.size()));

  auto *currentSkeleton = mSkeletonVector.get() + (mLastSkeleton * MaxNumBones);
  size_t dataSize = std::min(boneTransforms.size(), MaxNumBones);
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

void EditorRendererStorage::updateBuffers() {

  mCameraBuffer.update(&mCameraData);
  mEditorGridBuffer.update(&mEditorGridData);

  if (!mSkeletonTransforms.empty()) {
    mSkeletonTransformsBuffer.update(mSkeletonTransforms.data());
    mSkeletonBoneTransformsBuffer.update(mSkeletonVector.get());
  }

  mGizmoTransformsBuffer.update(mGizmoTransforms.data());

  mCollidableEntityBuffer.update(&mCollidableEntityParams);
}

void EditorRendererStorage::clear() {
  mSkeletonTransforms.clear();
  mGizmoTransforms.clear();
  mNumBones.clear();
  mGizmoCounts.clear();
  mLastSkeleton = 0;

  mCollidableEntity = liquid::EntityNull;
}

void EditorRendererStorage::setCollidable(
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
