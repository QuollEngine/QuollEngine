#pragma once

#include "liquid/rhi/ResourceRegistry.h"
#include "liquid/asset/MeshAsset.h"
#include "liquid/entity/Entity.h"
#include "liquid/renderer/Material.h"
#include "liquid/entity/EntityDatabase.h"

namespace liquid {

/**
 * @brief Render storage
 *
 * Stores everything necessary to
 * render the scene
 */
class RenderStorage {
  /**
   * Default reserved space for buffers
   */
  static constexpr size_t DEFAULT_RESERVED_SPACE = 10000;

  /**
   * Maximum number of joints
   */
  static constexpr size_t MAX_NUM_JOINTS = 32;

  /**
   * Maximum number of lights
   */
  static constexpr size_t MAX_NUM_LIGHTS = 256;

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
     * Light view projection matrix
     *
     * Used for shadow mapping
     */
    glm::mat4 lightMatrix;
  };

  struct SceneData {
    /**
     * Scene data
     *
     * First value is number of lights
     * Second value represents if IBL is active
     */
    glm::ivec4 data{0};
  };

  struct MeshData {
    /**
     * List of indices that point items in
     * storage
     */
    std::vector<uint32_t> indices;
  };

public:
  /**
   * @brief Create render storage
   *
   * @param reservedSpace Reserved space for buffer data
   */
  RenderStorage(size_t reservedSpace = DEFAULT_RESERVED_SPACE);

  /**
   * @brief Update storage buffers
   *
   * @param registry Resource registry
   */
  void updateBuffers(rhi::ResourceRegistry &registry);

  /**
   * @brief Get mesh transforms buffer
   *
   * @return Mesh transforms buffer
   */
  inline rhi::BufferHandle getMeshTransformsBuffer() const {
    return mMeshTransformsBuffer;
  }

  /**
   * @brief Get skinned mesh transforms buffer
   *
   * @return Skinned mesh transforms buffer
   */
  inline rhi::BufferHandle getSkinnedMeshTransformsBuffer() const {
    return mSkinnedMeshTransformsBuffer;
  }

  /**
   * @brief Get skeletons buffer
   *
   * @return Skeletons buffer
   */
  inline rhi::BufferHandle getSkeletonsBuffer() const {
    return mSkeletonsBuffer;
  }

  /**
   * @brief Get light buffer
   *
   * @return Light buffer
   */
  inline rhi::BufferHandle getSceneBuffer() const { return mSceneBuffer; }

  /**
   * @brief Get light buffer
   *
   * @return Light buffer
   */
  inline rhi::BufferHandle getLightsBuffer() const { return mLightsBuffer; }

  /**
   * @brief Get active camera buffer
   *
   * @return Active camera buffer
   */
  inline rhi::BufferHandle getActiveCameraBuffer() const {
    return mCameraBuffer;
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
  inline const std::unordered_map<SkinnedMeshAssetHandle, MeshData> &
  getSkinnedMeshGroups() const {
    return mSkinnedMeshGroups;
  }

  /**
   * @brief Get irradiance map
   *
   * @return Irradiance map
   */
  inline rhi::TextureHandle getIrradianceMap() const { return mIrradianceMap; }

  /**
   * @brief Get specular map
   *
   * @return Specular map
   */
  inline rhi::TextureHandle getSpecularMap() const { return mSpecularMap; }

  /**
   * @brief Get BRDF LUT
   *
   * @return BRDF LUT texture
   */
  inline rhi::TextureHandle getBrdfLUT() const { return mBrdfLUT; }

  /**
   * @brief Get number of lights
   *
   * @return Number of lights
   */
  inline int32_t getNumLights() const { return mSceneData.data.x; }

  /**
   * @brief Add mesh data
   *
   * @param handle Mesh handle
   * @param transform Mesh world transform
   */
  void addMesh(MeshAssetHandle handle, const glm::mat4 &transform);

  /**
   * @brief Add skinned mesh data
   *
   * @param handle Skinned mesh handle
   * @param transform Skinned mesh world transform
   * @param skeleton Skeleton joint transforms
   */
  void addSkinnedMesh(SkinnedMeshAssetHandle handle, const glm::mat4 &transform,
                      const std::vector<glm::mat4> &skeleton);

  /**
   * @brief Add directional light
   *
   * @param light Directional light component
   */
  void addLight(const DirectionalLightComponent &light);

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
   * @param data Camera component data
   */
  void setCameraData(const CameraComponent &data);

  /**
   * @brief Clear intermediary buffers
   */
  void clear();

private:
  std::vector<glm::mat4> mMeshTransformMatrices;
  std::vector<glm::mat4> mSkinnedMeshTransformMatrices;
  std::unique_ptr<glm::mat4> mSkeletonVector;
  std::vector<LightData> mLights;
  SceneData mSceneData{};
  CameraComponent mCameraData;

  size_t mLastSkeleton = 0;

  rhi::BufferHandle mMeshTransformsBuffer = rhi::BufferHandle::Invalid;
  rhi::BufferHandle mSkinnedMeshTransformsBuffer = rhi::BufferHandle::Invalid;
  rhi::BufferHandle mSkeletonsBuffer = rhi::BufferHandle::Invalid;
  rhi::BufferHandle mSceneBuffer = rhi::BufferHandle::Invalid;
  rhi::BufferHandle mLightsBuffer = rhi::BufferHandle::Invalid;
  rhi::BufferHandle mCameraBuffer = rhi::BufferHandle::Invalid;

  rhi::TextureHandle mIrradianceMap = rhi::TextureHandle::Invalid;
  rhi::TextureHandle mSpecularMap = rhi::TextureHandle::Invalid;
  rhi::TextureHandle mBrdfLUT = rhi::TextureHandle::Invalid;

  std::unordered_map<MeshAssetHandle, MeshData> mMeshGroups;
  std::unordered_map<SkinnedMeshAssetHandle, MeshData> mSkinnedMeshGroups;

  size_t mReservedSpace = 0;
};

} // namespace liquid
