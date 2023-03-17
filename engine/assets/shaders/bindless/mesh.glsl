#include "transform.glsl"

/**
 * @brief Single skeleton joints
 */
struct SkeletonItem {
  /**
   * Joints for skeleton
   */
  mat4 joints[32];
};

RegisterBuffer(std430, readonly, SkeletonData, { SkeletonItem items[]; });

#define getMeshTransform(index)                                                \
  GetBindlessResource(TransformData, uDrawParams.meshTransforms).items[index]

#define getSkinnedMeshTransform(index)                                         \
  GetBindlessResource(TransformData, uDrawParams.skinnedMeshTransforms)        \
      .items[index]

#define getSkeleton(index)                                                     \
  GetBindlessResource(SkeletonData, uDrawParams.skeletons).items[index]
