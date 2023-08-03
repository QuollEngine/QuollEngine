#pragma once

#include "liquid/rhi/RenderDevice.h"
#include "liquid/asset/MeshAsset.h"
#include "liquid/entity/Entity.h"
#include "liquid/renderer/Material.h"
#include "liquid/entity/EntityDatabase.h"
#include "liquid/renderer/BindlessDrawParameters.h"

namespace liquid {

/**
 * @brief Scene renderer frame data
 *
 * Stores everything necessary to
 * render a frame
 */
class SceneRendererFrameData {
public:
  /**
   * Default reserved space for buffers
   */
  static constexpr size_t DefaultReservedSpace = 10000;

  /**
   * Maximum number of joints
   */
  static constexpr size_t MaxNumJoints = 32;

  /**
   * Maximum number of lights
   */
  static constexpr size_t MaxNumLights = 256;

  /**
   * Maximum number of shadow maps
   */
  static constexpr size_t MaxShadowMaps = 16;

  /**
   * @brief Directional light data
   */
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

  /**
   * @brief Point light data
   */
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

  /**
   * @brief Shadow map data
   */
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

  /**
   * @brief Scene data
   */
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

  /**
   * @brief Skybox data
   */
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

  /**
   * @brief Mesh data
   */
  struct MeshData {

    /**
     * @brief Transforms of mesh entities
     */
    std::vector<glm::mat4> transforms;

    /**
     * @brief Ids of mesh entities
     */
    std::vector<Entity> entities;
  };

  /**
   * @brief Skinned mesh data
   */
  struct SkinnedMeshData : public MeshData {
    /**
     * @brief Skeleton bone transforms
     */
    std::unique_ptr<glm::mat4> skeletons;

    /**
     * Last skeleton index
     */
    size_t lastSkeleton = 0;

    /**
     * Skeleton capacity
     */
    size_t skeletonCapacity = 0;
  };

  /**
   * @brief Glyph data
   *
   * Used for storing glyph
   * data in object buffers
   */
  struct GlyphData {
    /**
     * Atlas bounds
     */
    glm::vec4 bounds;

    /**
     * Plane bounds
     */
    glm::vec4 planeBounds;
  };

  /**
   * @brief Text data
   */
  struct TextData {
    /**
     * Text index
     *
     * Used for finding transforms
     */
    uint32_t index = 0;

    /**
     * Glyph start position in glyphs buffer
     */
    uint32_t glyphStart = 0;

    /**
     * Text length
     */
    uint32_t length = 0;
  };

public:
  /**
   * @brief Create frame data
   *
   * @param renderStorage Render Storage
   * @param reservedSpace Reserved space for buffer data
   */
  SceneRendererFrameData(RenderStorage &renderStorage,
                         size_t reservedSpace = DefaultReservedSpace);

  /**
   * @brief Update storage buffers
   */
  void updateBuffers();

  /**
   * @brief Get sprite entities
   *
   * @return Sprite entities
   */
  inline const std::vector<Entity> &getSpriteEntities() const {
    return mSpriteEntities;
  }

  /**
   * @brief Get mesh groups
   *
   * @return Mesh groups
   */
  inline const std::unordered_map<MeshAssetHandle, MeshData> &
  getMeshGroups() const {
    return mMeshGroups;
  }

  /**
   * @brief Get skinned mesh groups
   *
   * @return Skinned mesh groups
   */
  inline const std::unordered_map<SkinnedMeshAssetHandle, SkinnedMeshData> &
  getSkinnedMeshGroups() const {
    return mSkinnedMeshGroups;
  }

  /**
   * @brief Get text groups
   *
   * @return Text groups
   */
  inline const std::unordered_map<FontAssetHandle, std::vector<TextData>> &
  getTextGroups() const {
    return mTextGroups;
  }

  /**
   * @brief Get text glyphs
   *
   * @return Text glyphs
   */
  inline const std::vector<GlyphData> &getTextGlyphs() const {
    return mTextGlyphs;
  }

  /**
   * @brief Get text entities
   *
   * @return Text entities
   */
  inline const std::vector<Entity> &getTextEntities() const {
    return mTextEntities;
  }

  /**
   * @brief Get number of lights
   *
   * @return Number of lights
   */
  inline uint32_t getNumLights() const { return mSceneData.data.x; }

  /**
   * @brief Get number shadow maps
   *
   * @return Number of shadow maps
   */
  inline const size_t getNumShadowMaps() const { return mShadowMaps.size(); }

  /**
   * @brief Add mesh data
   *
   * @param handle Mesh handle
   * @param entity Entity
   * @param transform Mesh world transform
   */
  void addMesh(MeshAssetHandle handle, liquid::Entity entity,
               const glm::mat4 &transform);

  /**
   * @brief Set BRDF lookup table
   *
   * @param brdfLut BRDF Lookup table
   */
  void setBrdfLookupTable(rhi::TextureHandle brdfLut);

  /**
   * @brief Add skinned mesh data
   *
   * @param handle Skinned mesh handle
   * @param entity Entity
   * @param transform Skinned mesh world transform
   * @param skeleton Skeleton joint transforms
   */
  void addSkinnedMesh(SkinnedMeshAssetHandle handle, Entity entity,
                      const glm::mat4 &transform,
                      const std::vector<glm::mat4> &skeleton);

  /**
   * @brief Add directional light
   *
   * @param light Directional light component
   */
  void addLight(const DirectionalLight &light);

  /**
   * @brief Add directional light with shadows
   *
   * @param light Directional light
   * @param shadowMap Cascaded shadow map
   */
  void addLight(const DirectionalLight &light,
                const CascadedShadowMap &shadowMap);

  /**
   * @brief Add point light
   *
   * @param light Point light component
   * @param transform World transform
   */
  void addLight(const PointLight &light, const WorldTransform &transform);

  /**
   * @brief Add sprite
   *
   * @param entity Entity
   * @param texture Texture handle
   * @param worldTransform World transform
   */
  void addSprite(Entity entity, rhi::TextureHandle texture,
                 const glm::mat4 &worldTransform);

  /**
   * @brief Add text
   *
   * @param entity Entity
   * @param fontHandle Font handle
   * @param glyphs Text glyphs
   * @param transform Text world transform
   */
  void addText(Entity entity, FontAssetHandle fontHandle,
               const std::vector<GlyphData> &glyphs,
               const glm::mat4 &transform);

  /**
   * @brief Set skybox texture
   *
   * @param skyboxTexture Skybox texture
   */
  void setSkyboxTexture(rhi::TextureHandle skyboxTexture);

  /**
   * @brief Set skybox color
   *
   * @param color Skybox color
   */
  void setSkyboxColor(const glm::vec4 &color);

  /**
   * @brief Set environment textures
   *
   * @param irradianceMap Irradiance map
   * @param specularMap Specular map
   */
  void setEnvironmentTextures(rhi::TextureHandle irradianceMap,
                              rhi::TextureHandle specularMap);

  /**
   * @brief Set environment color
   *
   * @param color Environment color
   */
  void setEnvironmentColor(const glm::vec4 &color);

  /**
   * @brief Set camera data
   *
   * @param camera Camera data
   * @param lens Camera lens data
   */
  void setCameraData(const Camera &camera, const PerspectiveLens &lens);

  /**
   * @brief Set shadow map texture
   *
   * @param shadowmap Shadoow map texture
   */
  void setShadowMapTexture(rhi::TextureHandle shadowmap);

  /**
   * @brief Clear intermediary buffers
   */
  void clear();

  /**
   * @brief Get reserved space
   *
   * @return Reserved space
   */
  inline size_t getReservedSpace() const { return mReservedSpace; }

  /**
   * @brief Get bindless parameters
   *
   * @return Bindless parameters
   */
  inline BindlessDrawParameters &getBindlessParams() { return mBindlessParams; }

  /**
   * @brief Get bindless parameters
   *
   * @return Bindless parameters
   */
  inline const BindlessDrawParameters &getBindlessParams() const {
    return mBindlessParams;
  }

  /**
   * @brief Get sprite transforms buffer
   *
   * @return Sprite transforms buffer
   */
  inline rhi::DeviceAddress getSpriteTransformsBuffer() const {
    return mSpriteTransformsBuffer.getAddress();
  }

  /**
   * @brief Get sprite textures buffer
   *
   * @return Sprite textures buffer
   */
  inline rhi::DeviceAddress getSpriteTexturesBuffer() const {
    return mSpriteTexturesBuffer.getAddress();
  }

  /**
   * @brief Get mesh transforms buffer
   *
   * @return Mesh transforms buffer
   */
  inline rhi::DeviceAddress getMeshTransformsBuffer() const {
    return mMeshTransformsBuffer.getAddress();
  }

  /**
   * @brief Get skinned mesh transforms buffer
   *
   * @return Skinned mesh transforms buffer
   */
  inline rhi::DeviceAddress getSkinnedMeshTransformsBuffer() const {
    return mSkinnedMeshTransformsBuffer.getAddress();
  }

  /**
   * @brief Get text transforms buffer
   *
   * @return Text transforms buffer
   */
  inline rhi::DeviceAddress getTextTransformsBuffer() const {
    return mTextTransformsBuffer.getAddress();
  }

  /**
   * @brief Get skeletons buffer
   *
   * @return Skeletons buffer
   */
  inline rhi::DeviceAddress getSkeletonsBuffer() const {
    return mSkeletonsBuffer.getAddress();
  }

  /**
   * @brief Get camera buffer
   *
   * @return Camera buffer
   */
  inline rhi::DeviceAddress getCameraBuffer() const {
    return mCameraBuffer.getAddress();
  }

  /**
   * @brief Get scene buffer
   *
   * @return Scene buffer
   */
  inline rhi::DeviceAddress getSceneBuffer() const {
    return mSceneBuffer.getAddress();
  }

  /**
   * @brief Get directional lights buffer
   *
   * @return Directional lights buffer
   */
  inline rhi::DeviceAddress getDirectionalLightsBuffer() const {
    return mDirectionalLightsBuffer.getAddress();
  }

  /**
   * @brief Get point lights buffer
   *
   * @return Point lights buffer
   */
  inline rhi::DeviceAddress getPointLightsBuffer() const {
    return mPointLightsBuffer.getAddress();
  }

  /**
   * @brief Get shadow maps buffer
   *
   * @return Shadow maps buffer
   */
  inline rhi::DeviceAddress getShadowMapsBuffer() const {
    return mShadowMapsBuffer.getAddress();
  }

  /**
   * @brief Get skybox buffer
   *
   * @return Skybox buffer
   */
  inline rhi::DeviceAddress getSkyboxBuffer() const {
    return mSkyboxBuffer.getAddress();
  }

  /**
   * @brief Get glyphs buffer
   *
   * @return Glyphs buffer
   */
  inline rhi::DeviceAddress getGlyphsBuffer() const {
    return mTextGlyphsBuffer.getAddress();
  }

private:
  /**
   * @brief Add cascaded shadow maps
   *
   * @param light Directional light
   * @param shadowMap Cascaded shadow map
   */
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

  size_t mLastSkeleton = 0;

  rhi::Buffer mMeshTransformsBuffer;
  rhi::Buffer mSkinnedMeshTransformsBuffer;
  rhi::Buffer mSkeletonsBuffer;
  rhi::Buffer mSceneBuffer;
  rhi::Buffer mDirectionalLightsBuffer;
  rhi::Buffer mPointLightsBuffer;
  rhi::Buffer mShadowMapsBuffer;
  rhi::Buffer mCameraBuffer;
  rhi::Buffer mSkyboxBuffer;

  std::unordered_map<MeshAssetHandle, MeshData> mMeshGroups;
  std::unordered_map<SkinnedMeshAssetHandle, SkinnedMeshData>
      mSkinnedMeshGroups;

  std::vector<glm::mat4> mSpriteTransforms;
  std::vector<rhi::TextureHandle> mSpriteTextures;
  std::vector<Entity> mSpriteEntities;
  rhi::Buffer mSpriteTransformsBuffer;
  rhi::Buffer mSpriteTexturesBuffer;

  std::vector<glm::mat4> mTextTransforms;
  std::vector<Entity> mTextEntities;
  rhi::Buffer mTextTransformsBuffer;
  std::vector<GlyphData> mTextGlyphs;
  rhi::Buffer mTextGlyphsBuffer;
  std::unordered_map<FontAssetHandle, std::vector<TextData>> mTextGroups;

  size_t mReservedSpace = 0;

  BindlessDrawParameters mBindlessParams;
};

} // namespace liquid
