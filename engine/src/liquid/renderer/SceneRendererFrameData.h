#pragma once

#include "liquid/rhi/RenderDevice.h"
#include "liquid/asset/MeshAsset.h"
#include "liquid/entity/Entity.h"
#include "liquid/renderer/Material.h"
#include "liquid/entity/EntityDatabase.h"
#include "liquid/renderer/DrawParameters.h"

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
   * @brief Light data
   */
  struct LightData {
    /**
     * Light data
     *
     * Changes based on type of light
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
     * @brief Scene data
     *
     * First parameter is number of lights
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
   * @param device Render device
   * @param reservedSpace Reserved space for buffer data
   */
  SceneRendererFrameData(RenderStorage &renderStorage,
                         rhi::RenderDevice *device,
                         size_t reservedSpace = DefaultReservedSpace);

  /**
   * @brief Update storage buffers
   */
  void updateBuffers();

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
  getTextGroups() {
    return mTextGroups;
  }

  /**
   * @brief Get irradiance map
   *
   * @return Irradiance map
   */
  inline rhi::TextureHandle getIrradianceMap() const { return mIrradianceMap; }

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
   * @brief Add text
   *
   * @param fontHandle Font handle
   * @param glyphs Text glyphs
   * @param transform Text world transform
   */
  void addText(FontAssetHandle fontHandle, const std::vector<GlyphData> &glyphs,
               const glm::mat4 &transform);

  /**
   * @brief Set environment textures
   *
   * @param irradianceMap Irradiance map
   * @param specularMap Specular map
   * @param brdfLUT BRDF LUT
   */
  void setEnvironmentTextures(rhi::TextureHandle irradianceMap,
                              rhi::TextureHandle specularMap,
                              rhi::TextureHandle brdfLUT);

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
   * @brief Get draw parameters
   *
   * @return Draw parameters
   */
  inline DrawParameters &getDrawParams() { return mDrawParams; }

  /**
   * @brief Get draw parameters
   *
   * @return Draw parameters
   */
  inline const DrawParameters &getDrawParams() const { return mDrawParams; }

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
  std::vector<LightData> mLights;
  std::vector<ShadowMapData> mShadowMaps;
  SceneData mSceneData{};
  Camera mCameraData;
  PerspectiveLens mCameraLens;

  size_t mLastSkeleton = 0;

  rhi::Buffer mMeshTransformsBuffer;
  rhi::Buffer mSkinnedMeshTransformsBuffer;
  rhi::Buffer mSkeletonsBuffer;
  rhi::Buffer mSceneBuffer;
  rhi::Buffer mLightsBuffer;
  rhi::Buffer mShadowMapsBuffer;
  rhi::Buffer mCameraBuffer;

  rhi::TextureHandle mIrradianceMap = rhi::TextureHandle::Invalid;
  rhi::TextureHandle mSpecularMap = rhi::TextureHandle::Invalid;
  rhi::TextureHandle mBrdfLUT = rhi::TextureHandle::Invalid;

  std::unordered_map<MeshAssetHandle, MeshData> mMeshGroups;
  std::unordered_map<SkinnedMeshAssetHandle, SkinnedMeshData>
      mSkinnedMeshGroups;

  std::vector<glm::mat4> mTextTransforms;
  rhi::Buffer mTextTransformsBuffer;
  std::vector<GlyphData> mTextGlyphs;
  rhi::Buffer mTextGlyphsBuffer;
  std::unordered_map<FontAssetHandle, std::vector<TextData>> mTextGroups;

  size_t mReservedSpace = 0;

  DrawParameters mDrawParams;
};

} // namespace liquid
