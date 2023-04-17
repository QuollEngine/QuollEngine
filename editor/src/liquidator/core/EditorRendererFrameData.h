
#pragma once

#include "liquid/rhi/RenderHandle.h"
#include "liquid/rhi/RenderDevice.h"
#include "liquid/asset/MeshAsset.h"

#include "liquid/scene/Camera.h"
#include "liquid/renderer/RenderStorage.h"
#include "liquid/renderer/BindlessDrawParameters.h"

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
     * Entity world transform matrix
     */
    glm::mat4 worldTransform;

    /**
     * Entity type
     */
    glm::uvec4 type;

    /**
     * Collidable parameters
     *
     * Parameters differ between different
     * shape types
     */
    glm::vec4 params;
  };

  /**
   * @brief Mesh outline
   */
  struct MeshOutline {
    /**
     * Vertex buffer
     */
    rhi::BufferHandle vertexBuffer;

    /**
     * Index buffer
     */
    rhi::BufferHandle indexBuffer;
    /**
     * Index buffer
     */
    std::vector<uint32_t> indexCounts;
    /**
     * Index offsets
     */
    std::vector<uint32_t> indexOffsets;
    /**
     * Vertex offsets
     */
    std::vector<uint32_t> vertexOffsets;
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
  void setEditorGrid(const glm::uvec4 &data);

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
   * @brief Add mesh outline
   *
   * @param mesh Mesh asset
   * @param worldTransform World transform
   */
  void addMeshOutline(const MeshAsset &mesh, const glm::mat4 &worldTransform);

  /**
   * @brief Add skinned mesh outline
   *
   * @param mesh Skinned mesh asset
   * @param skeleton Skeleton joints
   * @param worldTransform World transform
   */
  void addSkinnedMeshOutline(const SkinnedMeshAsset &mesh,
                             const std::vector<glm::mat4> &skeleton,
                             const glm::mat4 &worldTransform);

  /**
   * @brief Get mesh outlines
   *
   * @return Mesh outlines
   */
  inline const std::vector<MeshOutline> &getMeshOutlines() const {
    return mMeshOutlines;
  }

  /**
   * @brief Get last mesh index in outline data
   *
   * @return Last mesh index
   */
  inline size_t getOutlineMeshEnd() const { return mOutlineMeshEnd; }

  /**
   * @brief Get last skinned mesh index in outline data
   *
   * @return Last skinned mesh index
   */
  inline size_t getOutlineSkinnedMeshEnd() const {
    return mOutlineSkinnedMeshEnd;
  }

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
    return mCollidableEntity != Entity::Null;
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
   * @brief Get bindless parameters
   *
   * @return Bindless parameters
   */
  inline BindlessDrawParameters &getBindlessParams() { return mBindlessParams; }

private:
  size_t mReservedSpace = 0;

  // Outlines
  std::vector<MeshOutline> mMeshOutlines;
  size_t mOutlineMeshEnd = 0;
  size_t mOutlineSkinnedMeshEnd = 0;

  std::vector<glm::mat4> mOutlineTransforms;

  std::unique_ptr<glm::mat4> mOutlineSkeletons;
  size_t mLastOutlineSkeleton = 0;
  size_t mOutlineSkeletonCapacity = 0;
  rhi::Buffer mOutlineTransformsBuffer;
  rhi::Buffer mOutlineSkeletonsBuffer;

  // Camera
  Camera mCameraData;
  rhi::Buffer mCameraBuffer;

  // Editor grid
  glm::uvec4 mEditorGridData{};
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
  Entity mCollidableEntity = Entity::Null;
  CollidableEntity mCollidableEntityParams{};

  rhi::Buffer mCollidableEntityBuffer;

  BindlessDrawParameters mBindlessParams;
};

} // namespace liquid::editor
