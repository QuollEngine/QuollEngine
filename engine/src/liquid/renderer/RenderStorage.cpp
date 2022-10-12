#include "liquid/core/Base.h"

#include "liquid/rhi/RenderHandle.h"
#include "RenderStorage.h"

namespace liquid {

RenderStorage::RenderStorage(rhi::RenderDevice *device, size_t reservedSpace)
    : mReservedSpace(reservedSpace), mDevice(device) {
  mMeshTransformMatrices.reserve(mReservedSpace);

  mSkinnedMeshTransformMatrices.reserve(mReservedSpace);
  mSkeletonVector.reset(new glm::mat4[mReservedSpace * MaxNumJoints]);

  mLights.reserve(MaxNumLights);

  mTextTransforms.reserve(mReservedSpace);
  mTextGlyphs.reserve(mReservedSpace);

  mMeshEntities.reserve(mReservedSpace);
  mSkinnedMeshEntities.reserve(mReservedSpace);

  mMeshTransformsBuffer = mDevice->createBuffer(
      {rhi::BufferType::Storage, mReservedSpace * sizeof(glm::mat4)});

  mSkinnedMeshTransformsBuffer = mDevice->createBuffer(
      {rhi::BufferType::Storage, mReservedSpace * sizeof(glm::mat4)});

  mSkeletonsBuffer = mDevice->createBuffer(
      {rhi::BufferType::Storage,
       mReservedSpace * MaxNumJoints * sizeof(glm::mat4)});

  mTextTransformsBuffer = mDevice->createBuffer(
      {rhi::BufferType::Storage, mReservedSpace * sizeof(glm::mat4)});

  mTextGlyphsBuffer = mDevice->createBuffer(
      {rhi::BufferType::Storage, mReservedSpace * sizeof(GlyphData)});

  mLightsBuffer = mDevice->createBuffer(
      {rhi::BufferType::Storage, mLights.capacity() * sizeof(LightData)});

  mCameraBuffer = mDevice->createBuffer(
      {rhi::BufferType::Uniform, sizeof(CameraComponent)});

  mSceneBuffer =
      mDevice->createBuffer({rhi::BufferType::Uniform, sizeof(SceneData)});
}

void RenderStorage::updateBuffers() {
  mMeshTransformsBuffer.update(mMeshTransformMatrices.data());
  mSkinnedMeshTransformsBuffer.update(mSkinnedMeshTransformMatrices.data());
  mSkeletonsBuffer.update(mSkeletonVector.get());
  mTextTransformsBuffer.update(mTextTransforms.data());
  mTextGlyphsBuffer.update(mTextGlyphs.data());
  mLightsBuffer.update(mLights.data());
  mCameraBuffer.update(&mCameraData);
  mSceneBuffer.update(&mSceneData);
}

void RenderStorage::addMesh(MeshAssetHandle handle, liquid::Entity entity,
                            const glm::mat4 &transform) {
  mMeshTransformMatrices.push_back(transform);
  mMeshEntities.push_back(entity);
  uint32_t index = static_cast<uint32_t>(mMeshTransformMatrices.size() - 1);

  if (mMeshGroups.find(handle) == mMeshGroups.end()) {
    MeshData data{};
    mMeshGroups.insert_or_assign(handle, data);
  }

  mMeshGroups.at(handle).indices.push_back(index);
}

void RenderStorage::addSkinnedMesh(SkinnedMeshAssetHandle handle, Entity entity,
                                   const glm::mat4 &transform,
                                   const std::vector<glm::mat4> &skeleton) {
  mSkinnedMeshTransformMatrices.push_back(transform);
  mSkinnedMeshEntities.push_back(entity);

  uint32_t index =
      static_cast<uint32_t>(mSkinnedMeshTransformMatrices.size() - 1);

  if (mSkinnedMeshGroups.find(handle) == mSkinnedMeshGroups.end()) {
    MeshData data{};
    mSkinnedMeshGroups.insert_or_assign(handle, data);
  }

  mSkinnedMeshGroups.at(handle).indices.push_back(index);

  auto *currentSkeleton =
      mSkeletonVector.get() + (mLastSkeleton * MaxNumJoints);
  size_t dataSize = std::min(skeleton.size(), MaxNumJoints);
  memcpy(currentSkeleton, skeleton.data(), dataSize * sizeof(glm::mat4));

  mLastSkeleton++;
}

void RenderStorage::addLight(const DirectionalLightComponent &light) {
  // Calculate projection matrix
  const float DIR_LIGHT_SIZE = 20.0f;
  const float DIR_LIGHT_NEAR = 0.001f;
  const float DIR_LIGHT_FAR = 100.0f;
  const float DIR_LIGHT_Z = 0.01f;
  glm::vec3 mPosition{-light.direction - light.direction * DIR_LIGHT_SIZE};
  mPosition.z = DIR_LIGHT_Z;
  glm::mat4 lightProjectionMatrix =
      glm::ortho(-DIR_LIGHT_SIZE, DIR_LIGHT_SIZE, -DIR_LIGHT_SIZE,
                 DIR_LIGHT_SIZE, DIR_LIGHT_NEAR, DIR_LIGHT_FAR);
  glm::mat4 lightViewMatrix = glm::lookAt(
      mPosition,
      mPosition + light.direction - glm::vec3(0.0f, 0.0f, DIR_LIGHT_Z),
      {0.0f, 1.0f, 0.0f});

  auto projectionViewMatrix = lightProjectionMatrix * lightViewMatrix;

  LightData data{
      glm::vec4(light.direction, light.intensity),
      light.color,
      projectionViewMatrix,
  };
  mLights.push_back(data);

  mSceneData.data.x = static_cast<int32_t>(mLights.size());
}

void RenderStorage::addText(FontAssetHandle font,
                            const std::vector<GlyphData> &glyphs,
                            const glm::mat4 &transform) {
  mTextTransforms.push_back(transform);
  uint32_t index = static_cast<uint32_t>(mTextTransforms.size() - 1);

  TextData textData{};
  textData.index = index;
  textData.glyphStart = static_cast<uint32_t>(mTextGlyphs.size());
  textData.length = static_cast<uint32_t>(glyphs.size());

  for (auto &glyph : glyphs) {
    mTextGlyphs.push_back(glyph);
  }

  if (mTextGroups.find(font) == mTextGroups.end()) {
    mTextGroups.insert_or_assign(font, std::vector<TextData>{textData});
  } else {
    mTextGroups.at(font).push_back(textData);
  }
}

void RenderStorage::setEnvironmentTextures(rhi::TextureHandle irradianceMap,
                                           rhi::TextureHandle specularMap,
                                           rhi::TextureHandle brdfLUT) {
  mIrradianceMap = irradianceMap;
  mSpecularMap = specularMap;
  mBrdfLUT = brdfLUT;
  mSceneData.data.y = 1;
}

void RenderStorage::setCameraData(const CameraComponent &data) {
  mCameraData = data;
}

void RenderStorage::clear() {
  mMeshTransformMatrices.clear();
  mSkinnedMeshTransformMatrices.clear();
  mMeshEntities.clear();
  mSkinnedMeshEntities.clear();

  mTextTransforms.clear();
  mTextGroups.clear();
  mTextGlyphs.clear();

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
