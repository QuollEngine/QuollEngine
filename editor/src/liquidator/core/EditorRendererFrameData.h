
#pragma once

#include "liquid/rhi/RenderHandle.h"
#include "liquid/rhi/RenderDevice.h"

#include "liquid/scene/Camera.h"
#include "liquidator/editor-scene/EditorGrid.h"
#include "liquid/renderer/RenderStorage.h"
#include "liquid/renderer/DrawParameters.h"

#include "liquid/entity/EntityDatabase.h"

namespace liquid::editor {

/**
 * @brief Frame data for editor renderer
 *
 * Store data for each frame
 */
class EditorRendererFrameData {
public:
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
   * @param renderStorage Render storage
   * @param reservedSpace Reserved space for buffer data
   */
  EditorRendererFrameData(RenderStorage &renderStorage,
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
  inline rhi::BufferHandle getSkeletonTransforms() const {
    return mSkeletonTransformsBuffer.getHandle();
  };

  /**
   * @brief Get skeleton bones buffer
   *
   * @return Skeleton bones buffer
   */
  inline rhi::BufferHandle getSkeletonBoneTransforms() const {
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
  void setActiveCamera(const Camera &camera);

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
  inline rhi::BufferHandle getEditorGridBuffer() const {
    return mEditorGridBuffer.getHandle();
  }

  /**
   * @brief Get active camera buffer
   *
   * @return Active camera buffer
   */
  inline rhi::BufferHandle getActiveCameraBuffer() {
    return mCameraBuffer.getHandle();
  }

  /**
   * @brief Add gizmo
   *
   * @param icon Gizmo icon
   * @param worldTransform World transform
   */
  void addGizmo(rhi::TextureHandle icon, const glm::mat4 &worldTransform);

  /**
   * @brief Get buffer for gizmo world transforms
   *
   * @return Gizmo transforms
   */
  inline rhi::BufferHandle getGizmoTransformsBuffer() const {
    return mGizmoTransformsBuffer.getHandle();
  }

  /**
   * @brief Get gizmo counts
   *
   * @return Gizmo counts per icon
   */
  inline const std::unordered_map<rhi::TextureHandle, uint32_t> &
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
  void setCollidable(Entity entity, const Collidable &collidable,
                     const WorldTransform &worldTransform);

  /**
   * @brief Get collidable parameters buffer
   *
   * @return Collidable parameters buffer handle
   */
  inline rhi::BufferHandle getCollidableParamsBuffer() const {
    return mCollidableEntityBuffer.getHandle();
  }

  /**
   * @brief Check if entity is set
   *
   * @retval true Collidable entity is set
   * @retval false Collidable entity is not set
   */
  inline bool isCollidableEntitySelected() const {
    return mCollidableEntity != EntityNull;
  }

  /**
   * @brief Get collidable shape type
   *
   * @return Collidable shape type
   */
  inline PhysicsGeometryType getCollidableShapeType() const {
    return static_cast<PhysicsGeometryType>(mCollidableEntityParams.type.x);
  }

  /**
   * @brief Get draw parameters
   *
   * @return Draw parameters
   */
  inline DrawParameters &getDrawParams() { return mDrawParams; }

private:
  size_t mReservedSpace = 0;

  // Camera
  Camera mCameraData;
  rhi::Buffer mCameraBuffer;

  // Editor grid
  EditorGridData mEditorGridData{};
  rhi::Buffer mEditorGridBuffer;

  // Skeleton bones
  size_t mLastSkeleton = 0;
  std::vector<glm::mat4> mSkeletonTransforms;
  std::unique_ptr<glm::mat4> mSkeletonVector;
  std::vector<uint32_t> mNumBones;
  rhi::Buffer mSkeletonTransformsBuffer;
  rhi::Buffer mSkeletonBoneTransformsBuffer;

  // Gizmos
  std::vector<glm::mat4> mGizmoTransforms;
  std::unordered_map<rhi::TextureHandle, uint32_t> mGizmoCounts;
  rhi::Buffer mGizmoTransformsBuffer;

  // Collidable shape
  Entity mCollidableEntity = EntityNull;
  CollidableEntity mCollidableEntityParams{};

  rhi::Buffer mCollidableEntityBuffer;

  DrawParameters mDrawParams;
};

} // namespace liquid::editor
