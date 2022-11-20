#include "liquid/core/Base.h"

#include "liquid/rhi/RenderHandle.h"
#include "SceneRendererFrameData.h"

namespace liquid {

SceneRendererFrameData::SceneRendererFrameData(rhi::RenderDevice *device,
                                               size_t reservedSpace)
    : mReservedSpace(reservedSpace), mDevice(device) {

  mLights.reserve(MaxNumLights);

  mTextTransforms.reserve(mReservedSpace);
  mTextGlyphs.reserve(mReservedSpace);

  rhi::BufferDescription defaultDesc{};
  defaultDesc.type = rhi::BufferType::Storage;
  defaultDesc.size = mReservedSpace * sizeof(glm::mat4);
  defaultDesc.mapped = true;

  mMeshTransformsBuffer = mDevice->createBuffer(defaultDesc);
  mSkinnedMeshTransformsBuffer = mDevice->createBuffer(defaultDesc);
  mTextTransformsBuffer = mDevice->createBuffer(defaultDesc);

  {
    auto desc = defaultDesc;
    desc.size = mReservedSpace * MaxNumJoints * sizeof(glm::mat4);
    mSkeletonsBuffer = mDevice->createBuffer(desc);
  }

  {
    auto desc = defaultDesc;
    desc.size = mReservedSpace * sizeof(GlyphData);
    mTextGlyphsBuffer = mDevice->createBuffer(desc);
  }

  {
    auto desc = defaultDesc;
    desc.size = mLights.capacity() * sizeof(LightData);
    mLightsBuffer = mDevice->createBuffer(desc);
  }

  {
    auto desc = defaultDesc;
    desc.size = sizeof(Camera);
    desc.type = rhi::BufferType::Uniform;
    mCameraBuffer = mDevice->createBuffer(desc);
  }

  {
    auto desc = defaultDesc;
    desc.size = sizeof(SceneData);
    desc.type = rhi::BufferType::Uniform;
    mSceneBuffer = mDevice->createBuffer(desc);
  }
}

void SceneRendererFrameData::updateBuffers() {
  {
    size_t offset = 0;
    auto *bufferData = static_cast<glm::mat4 *>(mMeshTransformsBuffer.map());
    for (auto &[_, data] : mMeshGroups) {
      memcpy(bufferData + offset, data.transforms.data(),
             data.transforms.size() * sizeof(glm::mat4));
      offset += data.transforms.size();
    }
  }

  {
    size_t transformsOffset = 0;
    auto *transformsBuffer =
        static_cast<glm::mat4 *>(mSkinnedMeshTransformsBuffer.map());

    size_t skeletonsOffset = 0;
    auto *skeletonsBuffer = static_cast<glm::mat4 *>(mSkeletonsBuffer.map());
    for (auto &[_, data] : mSkinnedMeshGroups) {
      memcpy(transformsBuffer + transformsOffset, data.transforms.data(),
             data.transforms.size() * sizeof(glm::mat4));
      transformsOffset += data.transforms.size();

      memcpy(skeletonsBuffer + skeletonsOffset, data.skeletons.get(),
             data.lastSkeleton * MaxNumJoints * sizeof(glm::mat4));
      skeletonsOffset += data.lastSkeleton * MaxNumJoints;
    }
  }

  mTextTransformsBuffer.update(mTextTransforms.data(),
                               mTextTransforms.size() * sizeof(glm::mat4));
  mTextGlyphsBuffer.update(mTextGlyphs.data(),
                           mTextGlyphs.size() * sizeof(GlyphData));
  mLightsBuffer.update(mLights.data(), mLights.size() * sizeof(LightData));
  mCameraBuffer.update(&mCameraData, sizeof(Camera));
  mSceneBuffer.update(&mSceneData, sizeof(SceneData));
}

void SceneRendererFrameData::addMesh(MeshAssetHandle handle,
                                     liquid::Entity entity,
                                     const glm::mat4 &transform) {
  if (mMeshGroups.find(handle) == mMeshGroups.end()) {
    MeshData data{};
    mMeshGroups.insert_or_assign(handle, data);
  }

  mMeshGroups.at(handle).entities.push_back(entity);
  mMeshGroups.at(handle).transforms.push_back(transform);
}

void SceneRendererFrameData::addSkinnedMesh(
    SkinnedMeshAssetHandle handle, Entity entity, const glm::mat4 &transform,
    const std::vector<glm::mat4> &skeleton) {

  if (mSkinnedMeshGroups.find(handle) == mSkinnedMeshGroups.end()) {
    mSkinnedMeshGroups.insert({handle, SkinnedMeshData{}});
  }

  auto &group = mSkinnedMeshGroups.at(handle);

  group.entities.push_back(entity);
  group.transforms.push_back(transform);

  size_t currentOffset = group.lastSkeleton * MaxNumJoints;
  size_t newSize = currentOffset + MaxNumJoints;

  // Resize skeletons if new skeleton does not fit
  if (group.skeletonCapacity < newSize) {
    group.skeletonCapacity = newSize * 2;
    auto *newVector = new glm::mat4[group.skeletonCapacity];

    memcpy(newVector, group.skeletons.get(), currentOffset * sizeof(glm::mat4));
    group.skeletons.reset(newVector);
  }

  auto *currentSkeleton = group.skeletons.get() + currentOffset;
  size_t dataSize = std::min(skeleton.size(), MaxNumJoints);
  memcpy(currentSkeleton, skeleton.data(), dataSize * sizeof(glm::mat4));
  group.lastSkeleton++;
}

void SceneRendererFrameData::addLight(const DirectionalLight &light) {
  // Calculate projection matrix
  const float DIR_LIGHT_SIZE = 20.0f;
  const float DIR_LIGHT_NEAR = 0.001f;
  const float DIR_LIGHT_FAR = 100.0f;
  const float DIR_LIGHT_Z = 0.01f;
  glm::vec3 mPosition{-light.direction - light.direction * DIR_LIGHT_SIZE};
  mPosition.z = DIR_LIGHT_Z;
  glm::mat4 lightProjectionMatrix =
      glm::ortho(-DIR_LIGHT_SIZE, DIR_LIGHT_SIZE, DIR_LIGHT_SIZE,
                 -DIR_LIGHT_SIZE, DIR_LIGHT_NEAR, DIR_LIGHT_FAR);
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

void SceneRendererFrameData::addText(FontAssetHandle font,
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

void SceneRendererFrameData::setEnvironmentTextures(
    rhi::TextureHandle irradianceMap, rhi::TextureHandle specularMap,
    rhi::TextureHandle brdfLUT) {
  mIrradianceMap = irradianceMap;
  mSpecularMap = specularMap;
  mBrdfLUT = brdfLUT;
  mSceneData.data.y = 1;
}

void SceneRendererFrameData::setCameraData(const Camera &data) {
  mCameraData = data;
}

void SceneRendererFrameData::clear() {
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
