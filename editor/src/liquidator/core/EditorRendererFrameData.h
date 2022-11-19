
#pragma once

#include "liquid/rhi/RenderHandle.h"
#include "liquid/rhi/RenderDevice.h"

#include "liquid/scene/Camera.h"
#include "liquidator/editor-scene/EditorGrid.h"

#include "liquid/entity/EntityDatabase.h"

namespace liquidator {

/**
 * @brief Frame data for editor renderer
 *
 * Store data for each frame
 */
class EditorRendererFrameData {
  /**
   * @brief Maximum number of debug bones
   */
  static constexpr size_t MaxNumBones = 64;

  /**
   * @brief Default reserved space for buffers
   */
  static constexpr size_t DefaultReservedSpace = 2000;

  /**
   * @brief Collidable entity data for buffers
   */
  struct CollidableEntity {
    /**
     * @brief Entity world transform matrix
     */
    glm::mat4 worldTransform;

    /**
     * @brief Entity type
     */
    glm::uvec4 type;

    /**
     * @brief Collidable parameters
     *
     * Parameters differ between different
     * shape types
     */
    glm::vec4 params;
  };

public:
  /**
   * @brief Create frame data
   *
   * @param device Render device
   * @param reservedSpace Reserved space for buffer data
   */
  EditorRendererFrameData(liquid::rhi::RenderDevice *device,
                          size_t reservedSpace = DefaultReservedSpace);

  /**
   * @brief Add skeleton
   *
   * @param worldTransform World transform matrix
   * @param boneTransforms Skeleton bone transforms
   */
  void addSkeleton(const glm::mat4 &worldTransform,
                   const std::vector<glm::mat4> &boneTransforms);

  /**
   * @brief Get skeleton world transforms buffer
   *
   * @return Skeleton world transforms buffer
   */
  inline liquid::rhi::BufferHandle getSkeletonTransforms() const {
    return mSkeletonTransformsBuffer.getHandle();
  };

  /**
   * @brief Get skeleton bones buffer
   *
   * @return Skeleton bones buffer
   */
  inline liquid::rhi::BufferHandle getSkeletonBoneTransforms() const {
    return mSkeletonBoneTransformsBuffer.getHandle();
  }

  /**
   * @brief Get number of bones
   *
   * @return Number of bones
   */
  inline const std::vector<uint32_t> &getBoneCounts() const {
    return mNumBones;
  }

  /**
   * @brief Set active camera
   *
   * @param camera Active camera
   */
  void setActiveCamera(const liquid::Camera &camera);

  /**
   * @brief Set editor grid data
   *
   * @param data Editor grid data
   */
  void setEditorGrid(const EditorGridData &data);

  /**
   * @brief Get editor grid buffer
   *
   * @return Editor grid buffer
   */
  inline liquid::rhi::BufferHandle getEditorGridBuffer() const {
    return mEditorGridBuffer.getHandle();
  }

  /**
   * @brief Get active camera buffer
   *
   * @return Active camera buffer
   */
  inline liquid::rhi::BufferHandle getActiveCameraBuffer() {
    return mCameraBuffer.getHandle();
  }

  /**
   * @brief Add gizmo
   *
   * @param icon Gizmo icon
   * @param worldTransform World transform
   */
  void addGizmo(liquid::rhi::TextureHandle icon,
                const glm::mat4 &worldTransform);

  /**
   * @brief Get buffer for gizmo world transforms
   *
   * @return Gizmo transforms
   */
  inline liquid::rhi::BufferHandle getGizmoTransformsBuffer() const {
    return mGizmoTransformsBuffer.getHandle();
  }

  /**
   * @brief Get gizmo counts
   *
   * @return Gizmo counts per icon
   */
  inline const std::unordered_map<liquid::rhi::TextureHandle, uint32_t> &
  getGizmoCounts() const {
    return mGizmoCounts;
  }

  /**
   * @brief Update hardware buffer
   */
  void updateBuffers();

  /**
   * @brief Clear local buffer
   */
  void clear();

  /**
   * @brief Set collidable entity
   *
   * @param entity Entity
   * @param collidable Collidable component
   * @param worldTransform World transform
   */
  void setCollidable(liquid::Entity entity,
                     const liquid::Collidable &collidable,
                     const liquid::WorldTransform &worldTransform);

  /**
   * @brief Get collidable parameters buffer
   *
   * @return Collidable parameters buffer handle
   */
  inline liquid::rhi::BufferHandle getCollidableParamsBuffer() const {
    return mCollidableEntityBuffer.getHandle();
  }

  /**
   * @brief Check if entity is set
   *
   * @retval true Collidable entity is set
   * @retval false Collidable entity is not set
   */
  inline bool isCollidableEntitySelected() const {
    return mCollidableEntity != liquid::EntityNull;
  }

  /**
   * @brief Get render device
   *
   * @return Render device
   */
  inline liquid::rhi::RenderDevice *getRenderDevice() { return mDevice; }

  /**
   * @brief Get collidable shape type
   *
   * @return Collidable shape type
   */
  inline liquid::PhysicsGeometryType getCollidableShapeType() const {
    return static_cast<liquid::PhysicsGeometryType>(
        mCollidableEntityParams.type.x);
  }

private:
  size_t mReservedSpace = 0;

  // Camera
  liquid::Camera mCameraData;
  liquid::rhi::Buffer mCameraBuffer;

  // Editor grid
  EditorGridData mEditorGridData{};
  liquid::rhi::Buffer mEditorGridBuffer;

  // Skeleton bones
  size_t mLastSkeleton = 0;
  std::vector<glm::mat4> mSkeletonTransforms;
  std::unique_ptr<glm::mat4> mSkeletonVector;
  std::vector<uint32_t> mNumBones;
  liquid::rhi::Buffer mSkeletonTransformsBuffer;
  liquid::rhi::Buffer mSkeletonBoneTransformsBuffer;

  // Gizmos
  std::vector<glm::mat4> mGizmoTransforms;
  std::unordered_map<liquid::rhi::TextureHandle, uint32_t> mGizmoCounts;
  liquid::rhi::Buffer mGizmoTransformsBuffer;

  // Collidable shape
  liquid::Entity mCollidableEntity = liquid::EntityNull;
  CollidableEntity mCollidableEntityParams{};

  liquid::rhi::Buffer mCollidableEntityBuffer;

  liquid::rhi::RenderDevice *mDevice;
};

} // namespace liquidator
