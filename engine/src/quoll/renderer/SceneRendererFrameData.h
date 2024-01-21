#pragma once

#include "quoll/asset/Asset.h"
#include "quoll/asset/MeshAsset.h"
#include "quoll/entity/Entity.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/renderer/BindlessDrawParameters.h"
#include "quoll/renderer/Material.h"
#include "quoll/rhi/RenderDevice.h"
#include "quoll/scene/Camera.h"
#include "quoll/scene/CascadedShadowMap.h"
#include "quoll/scene/DirectionalLight.h"
#include "quoll/scene/PerspectiveLens.h"
#include "quoll/scene/PointLight.h"
#include "quoll/scene/WorldTransform.h"

namespace quoll {

/**
 * @brief Scene renderer frame data
 *
 * Stores everything necessary to
 * render a frame
 */
class SceneRendererFrameData {
public:
  static constexpr usize DefaultReservedSpace = 10000;

  static constexpr usize MaxNumJoints = 32;

  static constexpr usize MaxNumLights = 256;

  static constexpr usize MaxShadowMaps = 16;

  struct DirectionalLightData {
    /**
     * Light data
     *
     * First three values are direction
     * Last value is intensity
     */
    glm::vec4 data;

    /**
     * Light color
     */
    glm::vec4 color;

    /**
     * Shadow data
     *
     * First parameter indicates if shadows are enabled
     * Second parameter is the shadow index
     * Third parameter is the number of cascades
     *  which is used in cascaded shadow mapping
     */
    glm::uvec4 shadowData{0};
  };

  struct PointLightData {
    /**
     * Light data
     *
     * First three values are position
     * Last value is direction
     */
    glm::vec4 data;

    /**
     * Light range
     *
     * vec4 is used for padding purposes
     */
    glm::vec4 range;

    /**
     * Light color
     */
    glm::vec4 color;
  };

  struct ShadowMapData {
    /**
     * Shadow matrix generated from light
     */
    glm::mat4 shadowMatrix;

    /**
     * Shadow data
     *
     * First parameter indicates shadow split depth
     * Second parameter indicates if
     *  percentage closer filtering is enabled
     */
    glm::vec4 data;
  };

  struct SceneData {
    /**
     * Environment lighting type
     */
    enum EnvironmentLighting { None = 0, Color = 1, Texture = 2 };

    /**
     * Light data
     *
     * First parameter is number of directional lights
     * Second parameter is number of point lights
     *
     * Fourth parameter is environment lighting type
     *   (0 = none, 1 = color, 2 = texture)
     */
    glm::uvec4 data{0};

    /**
     * Scene textures
     *
     * First parameter is IBL irradiance map
     * Second parameter is IBL specular map
     * Third parameter is IBL BRDF LUT
     * Fourth parameter is shadow map
     */
    glm::uvec4 textures{0};

    /**
     * Scene color
     */
    glm::vec4 color;
  };

  struct SkyboxData {
    /**
     * Skybox data
     *
     * First value represents texture id
     */
    glm::uvec4 data{0};

    /**
     * Skybox color
     */
    glm::vec4 color;
  };

  struct MaterialRange {
    u32 start = 0;

    u32 end = 0;
  };

  struct MeshData {

    std::vector<glm::mat4> transforms;

    std::vector<MaterialRange> materialRanges;

    std::vector<Entity> entities;
  };

  struct SkinnedMeshData : public MeshData {
    std::unique_ptr<glm::mat4> skeletons;

    usize lastSkeleton = 0;

    usize skeletonCapacity = 0;
  };

  struct GlyphData {
    glm::vec4 atlasBounds;

    glm::vec4 planeBounds;
  };

  struct TextItem {
    rhi::TextureHandle fontTexture = rhi::TextureHandle::Null;

    u32 glyphStart = 0;

    u32 length = 0;
  };

public:
  SceneRendererFrameData(RenderStorage &renderStorage,
                         usize reservedSpace = DefaultReservedSpace);

  void updateBuffers();

  inline const std::vector<Entity> &getSpriteEntities() const {
    return mSpriteEntities;
  }

  inline const std::unordered_map<MeshAssetHandle, MeshData> &
  getMeshGroups() const {
    return mMeshGroups;
  }

  inline const std::unordered_map<MeshAssetHandle, SkinnedMeshData> &
  getSkinnedMeshGroups() const {
    return mSkinnedMeshGroups;
  }

  inline const std::vector<TextItem> &getTexts() const { return mTexts; }

  inline const std::vector<GlyphData> &getTextGlyphs() const {
    return mTextGlyphs;
  }

  inline const std::vector<Entity> &getTextEntities() const {
    return mTextEntities;
  }

  inline u32 getNumLights() const { return mSceneData.data.x; }

  inline const usize getNumShadowMaps() const { return mShadowMaps.size(); }

  void setDefaultMaterial(rhi::DeviceAddress material);

  void addMesh(MeshAssetHandle handle, quoll::Entity entity,
               const glm::mat4 &transform,
               const std::vector<rhi::DeviceAddress> &materials);

  void addSkinnedMesh(MeshAssetHandle handle, Entity entity,
                      const glm::mat4 &transform,
                      const std::vector<glm::mat4> &skeleton,
                      const std::vector<rhi::DeviceAddress> &materials);

  void setBrdfLookupTable(rhi::TextureHandle brdfLut);

  void addLight(const DirectionalLight &light);

  void addLight(const DirectionalLight &light,
                const CascadedShadowMap &shadowMap);

  void addLight(const PointLight &light, const WorldTransform &transform);

  void addSprite(Entity entity, rhi::TextureHandle texture,
                 const glm::mat4 &worldTransform);

  void addText(Entity entity, rhi::TextureHandle fontTexture,
               const std::vector<GlyphData> &glyphs,
               const glm::mat4 &transform);

  void setSkyboxTexture(rhi::TextureHandle skyboxTexture);

  void setSkyboxColor(const glm::vec4 &color);

  void setEnvironmentTextures(rhi::TextureHandle irradianceMap,
                              rhi::TextureHandle specularMap);

  void setEnvironmentColor(const glm::vec4 &color);

  void setCameraData(const Camera &camera, const PerspectiveLens &lens);

  void setShadowMapTexture(rhi::TextureHandle shadowmap);

  void clear();

  inline usize getReservedSpace() const { return mReservedSpace; }

  inline BindlessDrawParameters &getBindlessParams() { return mBindlessParams; }

  inline const BindlessDrawParameters &getBindlessParams() const {
    return mBindlessParams;
  }

  inline rhi::DeviceAddress getSpriteTransformsBuffer() const {
    return mSpriteTransformsBuffer.getAddress();
  }

  inline rhi::DeviceAddress getSpriteTexturesBuffer() const {
    return mSpriteTexturesBuffer.getAddress();
  }

  inline rhi::DeviceAddress getFlattenedMaterialsBuffer() const {
    return mFlatMaterialsBuffer.getAddress();
  }

  inline rhi::DeviceAddress getMeshTransformsBuffer() const {
    return mMeshTransformsBuffer.getAddress();
  }

  inline rhi::DeviceAddress getMeshMaterialsBuffer() const {
    return mMeshMaterialsBuffer.getAddress();
  }

  inline rhi::DeviceAddress getSkinnedMeshTransformsBuffer() const {
    return mSkinnedMeshTransformsBuffer.getAddress();
  }

  inline rhi::DeviceAddress getSkinnedMeshMaterialsBuffer() const {
    return mSkinnedMeshMaterialsBuffer.getAddress();
  }

  inline rhi::DeviceAddress getTextTransformsBuffer() const {
    return mTextTransformsBuffer.getAddress();
  }

  inline rhi::DeviceAddress getSkeletonsBuffer() const {
    return mSkeletonsBuffer.getAddress();
  }

  inline rhi::DeviceAddress getCameraBuffer() const {
    return mCameraBuffer.getAddress();
  }

  inline rhi::DeviceAddress getSceneBuffer() const {
    return mSceneBuffer.getAddress();
  }

  inline rhi::DeviceAddress getDirectionalLightsBuffer() const {
    return mDirectionalLightsBuffer.getAddress();
  }

  inline rhi::DeviceAddress getPointLightsBuffer() const {
    return mPointLightsBuffer.getAddress();
  }

  inline rhi::DeviceAddress getShadowMapsBuffer() const {
    return mShadowMapsBuffer.getAddress();
  }

  inline rhi::DeviceAddress getSkyboxBuffer() const {
    return mSkyboxBuffer.getAddress();
  }

  inline rhi::DeviceAddress getGlyphsBuffer() const {
    return mTextGlyphsBuffer.getAddress();
  }

private:
  void addCascadedShadowMaps(const DirectionalLight &light,
                             const CascadedShadowMap &shadowMap);

private:
  std::vector<DirectionalLightData> mDirectionalLights;
  std::vector<PointLightData> mPointLights;
  std::vector<ShadowMapData> mShadowMaps;
  SceneData mSceneData{};
  SkyboxData mSkyboxData{};
  Camera mCameraData;
  PerspectiveLens mCameraLens;

  usize mLastSkeleton = 0;

  std::vector<rhi::DeviceAddress> mFlatMaterials;
  rhi::Buffer mFlatMaterialsBuffer;

  rhi::Buffer mMeshTransformsBuffer;
  rhi::Buffer mSkinnedMeshTransformsBuffer;
  rhi::Buffer mSkeletonsBuffer;
  rhi::Buffer mMeshMaterialsBuffer;
  rhi::Buffer mSkinnedMeshMaterialsBuffer;
  std::unordered_map<MeshAssetHandle, MeshData> mMeshGroups;
  std::unordered_map<MeshAssetHandle, SkinnedMeshData> mSkinnedMeshGroups;

  rhi::Buffer mSceneBuffer;
  rhi::Buffer mDirectionalLightsBuffer;
  rhi::Buffer mPointLightsBuffer;
  rhi::Buffer mShadowMapsBuffer;
  rhi::Buffer mCameraBuffer;
  rhi::Buffer mSkyboxBuffer;

  std::vector<glm::mat4> mSpriteTransforms;
  std::vector<rhi::TextureHandle> mSpriteTextures;
  std::vector<Entity> mSpriteEntities;
  rhi::Buffer mSpriteTransformsBuffer;
  rhi::Buffer mSpriteTexturesBuffer;

  std::vector<TextItem> mTexts;
  std::vector<glm::mat4> mTextTransforms;
  std::vector<Entity> mTextEntities;
  std::vector<GlyphData> mTextGlyphs;

  rhi::Buffer mTextTransformsBuffer;
  rhi::Buffer mTextGlyphsBuffer;

  usize mReservedSpace = 0;

  BindlessDrawParameters mBindlessParams;
};

} // namespace quoll
