#include "../../../engine/assets/shaders/bindless/base.glsl"
#include "../../../engine/assets/shaders/bindless/transform.glsl"
#include "../../../engine/assets/shaders/bindless/camera.glsl"

/**
 * @brief Single debug skeleton transforms
 */
struct DebugSkeletonItem {
  /**
   * Bone matrices for skeleton
   */
  mat4 bones[64];
};

RegisterBuffer(std430, readonly, DebugSkeletonData,
               { DebugSkeletonItem items[]; });

RegisterUniform(GridData, { uvec4 gridLines; });

RegisterUniform(CollidableParamsUniform, {
  mat4 worldTransform;
  uvec4 type;
  vec4 params;
});

#define getGizmoTransform(index) getTransform(0, index)
#define GetDebugSkeleton(index)                                                \
  GetBindlessResourceFromPC(DebugSkeletonData, 1).items[index]
#define getGridData() GetBindlessResourceFromPC(GridData, 2)
#define getCamera() getCameraFromReg(3)
#define GetCollidableParams()                                                  \
  GetBindlessResourceFromPC(CollidableParamsUniform, 4)
#define getSkeletonTransform(index) getTransform(5, index)

#define getGizmoIcon() uGlobalTextures[pcDrawParameters.index8]
