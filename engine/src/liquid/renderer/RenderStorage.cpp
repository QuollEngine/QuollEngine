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
  mLights.reserve(MAX_NUM_LIGHTS);
}

void RenderStorage::updateBuffers(rhi::ResourceRegistry &registry) {
  LIQUID_ASSERT(rhi::isHandleValid(mCameraBuffer), "Camera is not set");

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

  mLightsBuffer = registry.setBuffer({rhi::BufferType::Storage,
                                      mLights.capacity() * sizeof(LightData),
                                      mLights.data()},
                                     mLightsBuffer);

  mSceneBuffer = registry.setBuffer(
      {rhi::BufferType::Uniform, sizeof(SceneData), &mSceneData}, mSceneBuffer);
}

void RenderStorage::addMesh(MeshAssetHandle handle,
                            const std::vector<SharedPtr<Material>> &materials,
                            const glm::mat4 &transform) {
  mMeshTransformMatrices.push_back(transform);
  uint32_t index = static_cast<uint32_t>(mMeshTransformMatrices.size() - 1);

  if (mMeshGroups.find(handle) == mMeshGroups.end()) {
    MeshData data{};
    data.materials = materials;
    mMeshGroups.insert_or_assign(handle, data);
  }

  mMeshGroups.at(handle).indices.push_back(index);
}

void RenderStorage::addSkinnedMesh(
    SkinnedMeshAssetHandle handle,
    const std::vector<SharedPtr<Material>> &materials,
    const glm::mat4 &transform, const std::vector<glm::mat4> &skeleton) {
  mSkinnedMeshTransformMatrices.push_back(transform);
  uint32_t index =
      static_cast<uint32_t>(mSkinnedMeshTransformMatrices.size() - 1);

  if (mSkinnedMeshGroups.find(handle) == mSkinnedMeshGroups.end()) {
    MeshData data{};
    data.materials = materials;
    mSkinnedMeshGroups.insert_or_assign(handle, data);
  }

  mSkinnedMeshGroups.at(handle).indices.push_back(index);

  auto *currentSkeleton =
      mSkeletonVector.get() + (mLastSkeleton * MAX_NUM_JOINTS);
  size_t dataSize = std::min(skeleton.size(), MAX_NUM_JOINTS);
  memcpy(currentSkeleton, skeleton.data(), dataSize * sizeof(glm::mat4));

  mLastSkeleton++;
}

void RenderStorage::addLight(const Light &light) {
  LightData data{
      glm::vec4(light.getDirection(), light.getIntensity()),
      light.getColor(),
      light.getProjectionViewMatrix(),
  };
  mLights.push_back(data);

  mSceneData.data.x = static_cast<int32_t>(mLights.size());
}

void RenderStorage::setEnvironmentTextures(rhi::TextureHandle irradianceMap,
                                           rhi::TextureHandle specularMap,
                                           rhi::TextureHandle brdfLUT) {
  mIrradianceMap = irradianceMap;
  mSpecularMap = specularMap;
  mBrdfLUT = brdfLUT;
  mSceneData.data.y = 1;
}

void RenderStorage::setActiveCamera(const SharedPtr<Camera> &camera) {
  mCameraBuffer = camera->getBuffer();
}

void RenderStorage::clear() {
  mMeshTransformMatrices.clear();
  mSkinnedMeshTransformMatrices.clear();
  mLights.clear();
  mSceneData.data.x = 0;
  mSceneData.data.y = 0;
  mLastSkeleton = 0;
  mIrradianceMap = rhi::TextureHandle::Invalid;
  mSpecularMap = rhi::TextureHandle::Invalid;
  mBrdfLUT = rhi::TextureHandle::Invalid;

  mMeshGroups.clear();
  mSkinnedMeshGroups.clear();
}

} // namespace liquid
