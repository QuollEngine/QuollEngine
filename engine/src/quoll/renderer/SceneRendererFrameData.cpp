#include "quoll/core/Base.h"

#include "quoll/rhi/RenderHandle.h"
#include "SceneRendererFrameData.h"

namespace quoll {

SceneRendererFrameData::SceneRendererFrameData(RenderStorage &renderStorage,
                                               usize reservedSpace)
    : mReservedSpace(reservedSpace),
      mBindlessParams(renderStorage.getDevice()
                          ->getDeviceInformation()
                          .getLimits()
                          .minUniformBufferOffsetAlignment) {
  mDirectionalLights.reserve(MaxNumLights);
  mPointLights.reserve(MaxNumLights);
  mShadowMaps.reserve(MaxShadowMaps);

  mTextTransforms.reserve(mReservedSpace);
  mTextGlyphs.reserve(mReservedSpace);
  mSpriteTransforms.reserve(mReservedSpace);
  mSpriteTextures.reserve(mReservedSpace);

  rhi::BufferDescription defaultDesc{};
  defaultDesc.usage = rhi::BufferUsage::Storage;
  defaultDesc.size = mReservedSpace * sizeof(glm::mat4);
  defaultDesc.mapped = true;

  {
    auto desc = defaultDesc;
    desc.debugName = "Flat materials";
    desc.size = mReservedSpace * sizeof(rhi::DeviceAddress);
    mFlatMaterialsBuffer = renderStorage.createBuffer(desc);
  }

  {
    auto desc = defaultDesc;
    desc.debugName = "Mesh transforms";
    mMeshTransformsBuffer = renderStorage.createBuffer(desc);
  }

  {
    auto desc = defaultDesc;
    desc.debugName = "Mesh material ranges";
    desc.size = mReservedSpace * sizeof(MaterialRange);
    mMeshMaterialsBuffer = renderStorage.createBuffer(desc);
  }

  {
    auto desc = defaultDesc;
    desc.debugName = "Skinned mesh transforms";
    mSkinnedMeshTransformsBuffer = renderStorage.createBuffer(desc);
  }

  {
    auto desc = defaultDesc;
    desc.debugName = "Skinned mesh material ranges";
    desc.size = mReservedSpace * sizeof(MaterialRange);
    mSkinnedMeshMaterialsBuffer = renderStorage.createBuffer(desc);
  }

  {
    auto desc = defaultDesc;
    desc.size = mReservedSpace * MaxNumJoints * sizeof(glm::mat4);
    desc.debugName = "Skeletons";

    mSkeletonsBuffer = renderStorage.createBuffer(desc);
  }

  {
    auto desc = defaultDesc;
    desc.debugName = "Text transforms";
    mTextTransformsBuffer = renderStorage.createBuffer(desc);
  }

  {
    auto desc = defaultDesc;
    desc.debugName = "Sprite transforms";
    mSpriteTransformsBuffer = renderStorage.createBuffer(desc);
  }

  {
    auto desc = defaultDesc;
    desc.size = mReservedSpace * sizeof(glm::uvec4);
    desc.debugName = "Sprite textures";
    mSpriteTexturesBuffer = renderStorage.createBuffer(desc);
  }

  {
    auto desc = defaultDesc;
    desc.size = mReservedSpace * sizeof(GlyphData);
    desc.debugName = "Text glyphs";

    mTextGlyphsBuffer = renderStorage.createBuffer(desc);
  }

  {
    auto desc = defaultDesc;
    desc.size = mDirectionalLights.capacity() * sizeof(DirectionalLightData);
    desc.debugName = "Directional lights";

    mDirectionalLightsBuffer = renderStorage.createBuffer(desc);
  }

  {
    auto desc = defaultDesc;
    desc.size = mPointLights.capacity() * sizeof(PointLightData);
    desc.debugName = "Point lights";

    mPointLightsBuffer = renderStorage.createBuffer(desc);
  }

  {
    auto desc = defaultDesc;
    desc.size = sizeof(Camera);
    desc.usage = rhi::BufferUsage::Uniform;
    desc.debugName = "Camera";

    mCameraBuffer = renderStorage.createBuffer(desc);
  }

  {
    auto desc = defaultDesc;
    desc.size = sizeof(SceneData);
    desc.usage = rhi::BufferUsage::Uniform;
    desc.debugName = "Scene";

    mSceneBuffer = renderStorage.createBuffer(desc);
  }

  {
    auto desc = defaultDesc;
    desc.size = sizeof(SkyboxData);
    desc.usage = rhi::BufferUsage::Uniform;
    desc.debugName = "Skybox";

    mSkyboxBuffer = renderStorage.createBuffer(desc);
  }

  {
    auto desc = defaultDesc;
    desc.size = mShadowMaps.capacity() * sizeof(ShadowMapData);
    desc.debugName = "Shadow maps";

    mShadowMapsBuffer = renderStorage.createBuffer(desc);
  }
}

void SceneRendererFrameData::updateBuffers() {
  QUOLL_PROFILE_EVENT("SceneRendererFrameData::updateBuffer");
  mFlatMaterialsBuffer.update(mFlatMaterials.data(),
                              mFlatMaterials.size() *
                                  sizeof(rhi::DeviceAddress));
  {
    usize offset = 0;
    auto *bufferData = static_cast<glm::mat4 *>(mMeshTransformsBuffer.map());
    for (auto &[_, data] : mMeshGroups) {
      memcpy(bufferData + offset, data.transforms.data(),
             data.transforms.size() * sizeof(glm::mat4));
      offset += data.transforms.size();
    }
  }

  {
    usize offset = 0;
    auto *bufferData = static_cast<MaterialRange *>(mMeshMaterialsBuffer.map());
    for (auto &[_, data] : mMeshGroups) {
      memcpy(bufferData + offset, data.materialRanges.data(),
             data.materialRanges.size() * sizeof(MaterialRange));
      offset += data.materialRanges.size();
    }
  }

  {
    usize transformsOffset = 0;
    auto *transformsBuffer =
        static_cast<glm::mat4 *>(mSkinnedMeshTransformsBuffer.map());

    usize skeletonsOffset = 0;
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

  {
    usize offset = 0;
    auto *bufferData =
        static_cast<MaterialRange *>(mSkinnedMeshMaterialsBuffer.map());
    for (auto &[_, data] : mSkinnedMeshGroups) {
      memcpy(bufferData + offset, data.materialRanges.data(),
             data.materialRanges.size() * sizeof(MaterialRange));
      offset += data.materialRanges.size();
    }
  }

  mTextTransformsBuffer.update(mTextTransforms.data(),
                               mTextTransforms.size() * sizeof(glm::mat4));
  mTextGlyphsBuffer.update(mTextGlyphs.data(),
                           mTextGlyphs.size() * sizeof(GlyphData));
  mDirectionalLightsBuffer.update(mDirectionalLights.data(),
                                  mDirectionalLights.size() *
                                      sizeof(DirectionalLightData));
  mPointLightsBuffer.update(mPointLights.data(),
                            mPointLights.size() * sizeof(PointLightData));
  mShadowMapsBuffer.update(mShadowMaps.data(),
                           mShadowMaps.size() * sizeof(ShadowMapData));
  mCameraBuffer.update(&mCameraData, sizeof(Camera));
  mSceneBuffer.update(&mSceneData, sizeof(SceneData));
  mSkyboxBuffer.update(&mSkyboxData, sizeof(SkyboxData));

  mSpriteTexturesBuffer.update(mSpriteTextures.data(),
                               mSpriteTextures.size() * sizeof(u32));
  mSpriteTransformsBuffer.update(mSpriteTransforms.data(),
                                 mSpriteTransforms.size() * sizeof(glm::mat4));
}

void SceneRendererFrameData::setDefaultMaterial(rhi::DeviceAddress material) {
  mFlatMaterials.at(0) = material;
}

void SceneRendererFrameData::addMesh(
    MeshAssetHandle handle, quoll::Entity entity, const glm::mat4 &transform,
    const std::vector<rhi::DeviceAddress> &materials) {
  u32 start = static_cast<u32>(mFlatMaterials.size());
  for (const auto &material : materials) {
    mFlatMaterials.push_back(material);
  }
  auto newMaterialSize = static_cast<u32>(mFlatMaterials.size());
  u32 end = newMaterialSize == start ? 0 : newMaterialSize - 1;

  if (mMeshGroups.find(handle) == mMeshGroups.end()) {
    MeshData data{};
    mMeshGroups.insert_or_assign(handle, data);
  }

  mMeshGroups.at(handle).entities.push_back(entity);
  mMeshGroups.at(handle).transforms.push_back(transform);
  mMeshGroups.at(handle).materialRanges.push_back({start, end});
}

void SceneRendererFrameData::addSkinnedMesh(
    MeshAssetHandle handle, Entity entity, const glm::mat4 &transform,
    const std::vector<glm::mat4> &skeleton,
    const std::vector<rhi::DeviceAddress> &materials) {
  u32 start = static_cast<u32>(mFlatMaterials.size());
  for (const auto &material : materials) {
    mFlatMaterials.push_back(material);
  }
  auto newMaterialSize = static_cast<u32>(mFlatMaterials.size());
  u32 end = newMaterialSize == start ? 0 : newMaterialSize - 1;

  if (mSkinnedMeshGroups.find(handle) == mSkinnedMeshGroups.end()) {
    mSkinnedMeshGroups.insert({handle, SkinnedMeshData{}});
  }

  auto &group = mSkinnedMeshGroups.at(handle);

  group.entities.push_back(entity);
  group.transforms.push_back(transform);
  group.materialRanges.push_back({start, end});

  usize currentOffset = group.lastSkeleton * MaxNumJoints;
  usize newSize = currentOffset + MaxNumJoints;

  // Resize skeletons if new skeleton does not fit
  if (group.skeletonCapacity < newSize) {
    group.skeletonCapacity = newSize * 2;
    auto *newVector = new glm::mat4[group.skeletonCapacity];

    memcpy(newVector, group.skeletons.get(), currentOffset * sizeof(glm::mat4));
    group.skeletons.reset(newVector);
  }

  auto *currentSkeleton = group.skeletons.get() + currentOffset;
  usize dataSize = std::min(skeleton.size(), MaxNumJoints);
  memcpy(currentSkeleton, skeleton.data(), dataSize * sizeof(glm::mat4));
  group.lastSkeleton++;
}

void SceneRendererFrameData::setBrdfLookupTable(rhi::TextureHandle brdfLut) {
  mSceneData.textures.z = static_cast<u32>(brdfLut);
}

void SceneRendererFrameData::addCascadedShadowMaps(
    const DirectionalLight &light, const CascadedShadowMap &shadowMap) {
  u32 numCascades = static_cast<u32>(shadowMap.numCascades);

  // Calculate split distances by combining
  // logarithmic and uniform splitting
  //
  // log_i = near * (far/near)^(i / size)
  // uniform_i = near + (far - near) * (1 / size)
  // distance_i = lambda * log_i + (1 - lambda) * uniform_i
  std::array<f32, CascadedShadowMap::MaxCascades + 1> splitDistances{};

  f32 splitLambda = shadowMap.splitLambda;

  f32 far = mCameraLens.far;
  f32 near = mCameraLens.near;

  f32 range = far - near;
  f32 ratio = far / near;

  const f32 fovY =
      2.0f * atanf(mCameraLens.sensorSize.y / (2.0f * mCameraLens.focalLength));

  for (usize i = 0; i < static_cast<usize>(numCascades + 1); ++i) {
    f32 p = static_cast<f32>(i + 1) / static_cast<f32>(splitDistances.size());
    f32 log = near * std::pow(ratio, p);
    f32 uniform = near + range * p;
    f32 d = splitLambda * log + (1.0f - splitLambda) * uniform;

    splitDistances.at(i) = mCameraLens.far * ((d - near) / range);
  }

  // Camera frustum NDC coordinates
  static constexpr std::array<glm::vec3, 8> FrustumCornersNDC{
      glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, -1.0f),
      glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, -1.0f, -1.0f),
      glm::vec3(-1.0f, 1.0f, 1.0f),  glm::vec3(1.0f, 1.0f, 1.0f),
      glm::vec3(1.0f, -1.0f, 1.0f),  glm::vec3(-1.0f, -1.0f, 1.0f),
  };

  f32 prevSplitDistance = mCameraLens.near;
  for (usize i = 0; i < numCascades; ++i) {
    f32 splitDistance = splitDistances.at(i);

    auto splitProjectionMatrix = glm::perspective(
        fovY, mCameraLens.aspectRatio, prevSplitDistance, splitDistance);

    auto invProjView =
        glm::inverse(splitProjectionMatrix * mCameraData.viewMatrix);

    std::array<glm::vec4, FrustumCornersNDC.size()> frustumCorners{};

    for (usize i = 0; i < frustumCorners.size(); ++i) {
      auto pt = invProjView * glm::vec4(FrustumCornersNDC.at(i), 1.0f);
      frustumCorners.at(i) = pt / pt.w;
    }

    // Calculate frustum center by adding
    // each corner and dividing the result by
    // number of corners
    glm::vec3 frustumCenter{0.0f};
    for (const auto &v : frustumCorners) {
      frustumCenter += glm::vec3(v);
    }
    frustumCenter /= static_cast<f32>(frustumCorners.size());

    f32 radius = 0.0f;
    for (const auto &corner : frustumCorners) {
      f32 distance = glm::length(glm::vec3(corner) - frustumCenter);
      radius = glm::max(radius, distance);
    }

    static constexpr f32 Sixteen = 16.0f;
    radius = std::ceil(radius * Sixteen) / Sixteen;
    glm::vec3 maxBounds = glm::vec3(radius);
    glm::vec3 minBounds = -maxBounds;

    f32 cascadeZ = maxBounds.z - minBounds.z;
    auto lightViewMatrix =
        glm::lookAt(frustumCenter - light.direction * radius, frustumCenter,
                    glm::vec3(0.0f, 1.0f, 0.0f));
    auto lightProjectionMatrix = glm::ortho(
        minBounds.x, maxBounds.x, minBounds.y, maxBounds.y, 0.0f, cascadeZ);

    auto projectionViewMatrix = lightProjectionMatrix * lightViewMatrix;

    mShadowMaps.push_back(
        {projectionViewMatrix,
         glm::vec4{-splitDistance, shadowMap.softShadows ? 1.0f : 0.0f, 0.0f,
                   0.0f}});

    prevSplitDistance = splitDistance;
  }
}

void SceneRendererFrameData::addLight(const DirectionalLight &light,
                                      const CascadedShadowMap &shadowMap) {
  u32 shadowIndex = static_cast<u32>(mShadowMaps.size());
  u32 numCascades = static_cast<u32>(shadowMap.numCascades);

  bool canCastShadows =
      (shadowIndex + numCascades) <= static_cast<u32>(MaxShadowMaps);

  if (canCastShadows) {
    addCascadedShadowMaps(light, shadowMap);
  }

  DirectionalLightData data{
      glm::vec4(light.direction, light.intensity), light.color,
      glm::uvec4(canCastShadows ? 1 : 0, shadowIndex, numCascades, 0)};
  mDirectionalLights.push_back(data);
  mSceneData.data.x = static_cast<i32>(mDirectionalLights.size());
}

void SceneRendererFrameData::addLight(const DirectionalLight &light) {
  DirectionalLightData data{glm::vec4(light.direction, light.intensity),
                            light.color};
  mDirectionalLights.push_back(data);

  mSceneData.data.x = static_cast<i32>(mDirectionalLights.size());
}

void SceneRendererFrameData::addLight(const PointLight &light,
                                      const WorldTransform &transform) {
  glm::vec3 scale;
  glm::quat orientation;
  glm::vec3 position;
  glm::vec3 skew;
  glm::vec4 perspective;
  glm::decompose(transform.worldTransform, scale, orientation, position, skew,
                 perspective);

  PointLightData data{glm::vec4(position, light.intensity),
                      glm::vec4(light.range), glm::vec4(light.color)};
  mPointLights.push_back(data);
  mSceneData.data.y = static_cast<i32>(mPointLights.size());
}

void SceneRendererFrameData::addSprite(Entity entity,
                                       rhi::TextureHandle texture,
                                       const glm::mat4 &worldTransform) {
  mSpriteEntities.push_back(entity);
  mSpriteTransforms.push_back(worldTransform);
  mSpriteTextures.push_back(texture);
}

void SceneRendererFrameData::addText(Entity entity,
                                     rhi::TextureHandle fontTexture,
                                     const std::vector<GlyphData> &glyphs,
                                     const glm::mat4 &transform) {
  mTextTransforms.push_back(transform);
  mTextEntities.push_back(entity);

  TextItem textData{};
  textData.fontTexture = fontTexture;
  textData.glyphStart = static_cast<u32>(mTextGlyphs.size());
  textData.length = static_cast<u32>(glyphs.size());

  for (auto &glyph : glyphs) {
    mTextGlyphs.push_back(glyph);
  }

  mTexts.push_back(textData);
}

void SceneRendererFrameData::setSkyboxTexture(rhi::TextureHandle texture) {
  mSkyboxData.data.x = static_cast<u32>(texture);
}

void SceneRendererFrameData::setSkyboxColor(const glm::vec4 &color) {
  mSkyboxData.color = color;
}

void SceneRendererFrameData::setEnvironmentTextures(
    rhi::TextureHandle irradianceMap, rhi::TextureHandle specularMap) {
  mSceneData.data.w = SceneData::EnvironmentLighting::Texture;
  mSceneData.textures.x = rhi::castHandleToUint(irradianceMap);
  mSceneData.textures.y = rhi::castHandleToUint(specularMap);
}

void SceneRendererFrameData::setEnvironmentColor(const glm::vec4 &color) {
  mSceneData.data.w = SceneData::EnvironmentLighting::Color;
  mSceneData.color = color;
}

void SceneRendererFrameData::setCameraData(const Camera &data,
                                           const PerspectiveLens &lens) {
  mCameraData = data;
  mCameraLens = lens;
}

void SceneRendererFrameData::setShadowMapTexture(rhi::TextureHandle shadowmap) {
  mSceneData.textures.w = rhi::castHandleToUint(shadowmap);
}

void SceneRendererFrameData::clear() {

  mSpriteEntities.clear();
  mSpriteTransforms.clear();
  mSpriteTextures.clear();

  mTextEntities.clear();
  mTextTransforms.clear();
  mTexts.clear();
  mTextGlyphs.clear();

  mDirectionalLights.clear();
  mPointLights.clear();
  mShadowMaps.clear();
  mSceneData.data.x = 0;
  mSceneData.data.y = 0;
  mSceneData.textures.x = 0;
  mSceneData.textures.y = 0;
  // Do not unset remove BRDF Lookup table
  mSceneData.color = {};
  mSkyboxData.color = {};
  mSkyboxData.data.x = 0;

  mLastSkeleton = 0;
  mFlatMaterials.clear();
  mFlatMaterials.resize(1);
  mMeshGroups.clear();
  mSkinnedMeshGroups.clear();
}

} // namespace quoll
