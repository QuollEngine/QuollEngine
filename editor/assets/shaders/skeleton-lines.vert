#version 460
#extension GL_ARB_separate_shader_objects : enable

#include "bindless-editor.glsl"

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

#define GetDebugSkeleton(index)                                                \
  GetBindlessResource(DebugSkeletonData, uDrawParams.debugSkeletons)           \
      .items[index]

#define getSkeletonTransform(index)                                            \
  GetBindlessResource(TransformData, uDrawParams.skeletonTransforms)           \
      .items[index]

layout(set = 1, binding = 0) uniform DrawParameters {
  uint gizmoTransforms;
  uint skeletonTransforms;
  uint debugSkeletons;
  uint collidbaleParams;
  uint camera;
  uint gridData;
}
uDrawParams;

void main() {
  gl_Position = getCamera().viewProj *
                getSkeletonTransform(gl_BaseInstance).modelMatrix *
                GetDebugSkeleton(gl_BaseInstance).bones[gl_VertexIndex] *
                vec4(0.0, 0.0, 0.0, 1.0);
}
