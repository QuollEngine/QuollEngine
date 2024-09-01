
#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "quoll/physics/Collidable.h"
#include "quoll/renderer/BindlessDrawParameters.h"
#include "quoll/renderer/MeshDrawData.h"
#include "quoll/renderer/RenderStorage.h"
#include "quoll/renderer/SceneRendererFrameData.h"
#include "quoll/rhi/RenderDevice.h"
#include "quoll/rhi/RenderHandle.h"
#include "quoll/scene/Camera.h"

namespace quoll::editor {

class EditorRendererFrameData {
public:
  static constexpr usize MaxNumBones = 64;

  static constexpr usize DefaultReservedSpace = 2000;

  struct CollidableEntity {
    glm::mat4 worldTransform;

    glm::uvec4 type;

    glm::vec4 center;

    glm::vec4 params;
  };

  struct MeshOutline {
    std::vector<rhi::BufferHandle> vertexBuffers;

    std::vector<u64> vertexBufferOffsets;

    rhi::BufferHandle indexBuffer;

    std::vector<u32> indexCounts;

    std::vector<u32> indexOffsets;

    std::vector<u32> vertexOffsets;
  };

public:
  EditorRendererFrameData(RenderStorage &renderStorage,
                          usize reservedSpace = DefaultReservedSpace);

  void addSkeleton(const glm::mat4 &worldTransform,
                   const std::vector<glm::mat4> &boneTransforms);

  inline rhi::BufferHandle getSkeletonTransforms() const {
    return mSkeletonTransformsBuffer.getHandle();
  };

  inline rhi::BufferHandle getSkeletonBoneTransforms() const {
    return mSkeletonBoneTransformsBuffer.getHandle();
  }

  inline const std::vector<u32> &getBoneCounts() const { return mNumBones; }

  void setActiveCamera(const Camera &camera);

  void setEditorGrid(const glm::uvec4 &data);

  inline rhi::BufferHandle getEditorGridBuffer() const {
    return mEditorGridBuffer.getHandle();
  }

  inline rhi::BufferHandle getActiveCameraBuffer() {
    return mCameraBuffer.getHandle();
  }

  void addGizmo(rhi::TextureHandle icon, const glm::mat4 &worldTransform);

  void addSpriteOutline(const glm::mat4 &worldTransform);

  void
  addTextOutline(rhi::TextureHandle fontTexture,
                 const std::vector<SceneRendererFrameData::GlyphData> &glyphs,
                 const glm::mat4 &worldTransform);

  void addMeshOutline(const MeshDrawData &drawData,
                      const glm::mat4 &worldTransform);

  void addSkinnedMeshOutline(const MeshDrawData &drawData,
                             const std::vector<glm::mat4> &skeleton,
                             const glm::mat4 &worldTransform);

  inline const std::vector<MeshOutline> &getMeshOutlines() const {
    return mMeshOutlines;
  }

  inline const std::vector<SceneRendererFrameData::TextItem> &
  getTextOutlines() const {
    return mTextOutlines;
  }

  inline usize getOutlineSpriteEnd() const { return mOutlineSpriteEnd; }

  inline usize getOutlineTextEnd() const { return mOutlineTextEnd; }

  inline usize getOutlineMeshEnd() const { return mOutlineMeshEnd; }

  inline usize getOutlineSkinnedMeshEnd() const {
    return mOutlineSkinnedMeshEnd;
  }

  inline rhi::BufferHandle getGizmoTransformsBuffer() const {
    return mGizmoTransformsBuffer.getHandle();
  }

  inline const std::unordered_map<rhi::TextureHandle, u32> &
  getGizmoCounts() const {
    return mGizmoCounts;
  }

  void updateBuffers();

  void clear();

  void setCollidable(Entity entity, const Collidable &collidable,
                     const WorldTransform &worldTransform);

  inline rhi::BufferHandle getCollidableParamsBuffer() const {
    return mCollidableEntityBuffer.getHandle();
  }

  inline bool isCollidableEntitySelected() const {
    return mCollidableEntity != Entity::Null;
  }

  inline PhysicsGeometryType getCollidableShapeType() const {
    return static_cast<PhysicsGeometryType>(mCollidableEntityParams.type.x);
  }

  inline BindlessDrawParameters &getBindlessParams() { return mBindlessParams; }

  void createBindlessParamsRange();

private:
  usize mReservedSpace = 0;

  // Outlines
  std::vector<glm::mat4> mOutlineTransforms;
  rhi::Buffer mOutlineTransformsBuffer;

  usize mOutlineSpriteEnd = 0;

  usize mOutlineTextEnd = 0;
  std::vector<SceneRendererFrameData::TextItem> mTextOutlines;
  std::vector<SceneRendererFrameData::GlyphData> mTextGlyphOutlines;
  rhi::Buffer mOutlineTextGlyphsBuffer;

  usize mOutlineMeshEnd = 0;
  std::vector<MeshOutline> mMeshOutlines;

  usize mOutlineSkinnedMeshEnd = 0;
  std::unique_ptr<glm::mat4> mOutlineSkeletons;
  usize mLastOutlineSkeleton = 0;
  usize mOutlineSkeletonCapacity = 0;
  rhi::Buffer mOutlineSkeletonsBuffer;

  // Camera
  Camera mCameraData;
  rhi::Buffer mCameraBuffer;

  // Editor grid
  glm::uvec4 mEditorGridData{};
  rhi::Buffer mEditorGridBuffer;

  // Skeleton bones
  usize mLastSkeleton = 0;
  std::vector<glm::mat4> mSkeletonTransforms;
  std::unique_ptr<glm::mat4> mSkeletonVector;
  std::vector<u32> mNumBones;
  rhi::Buffer mSkeletonTransformsBuffer;
  rhi::Buffer mSkeletonBoneTransformsBuffer;

  // Gizmos
  std::vector<glm::mat4> mGizmoTransforms;
  std::unordered_map<rhi::TextureHandle, u32> mGizmoCounts;
  rhi::Buffer mGizmoTransformsBuffer;

  // Collidable shape
  Entity mCollidableEntity = Entity::Null;
  CollidableEntity mCollidableEntityParams{};

  rhi::Buffer mCollidableEntityBuffer;

  BindlessDrawParameters mBindlessParams;
};

} // namespace quoll::editor
