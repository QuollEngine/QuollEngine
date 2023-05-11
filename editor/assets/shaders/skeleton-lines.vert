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

Buffer(64) DebugSkeletonsArray { DebugSkeletonItem items[]; };

layout(set = 0, binding = 0) uniform DrawParameters {
  Empty gizmoTransforms;
  TransformsArray skeletonTransforms;
  DebugSkeletonsArray debugSkeletons;
  Empty collidableParams;
  Camera camera;
  Empty gridData;
  Empty pad0;
  Empty pad1;
}
uDrawParams;

#define GetDebugSkeleton(index) uDrawParams.debugSkeletons.items[index]

#define getSkeletonTransform(index) uDrawParams.skeletonTransforms.items[index]

void main() {
  gl_Position = getCamera().viewProj *
                getSkeletonTransform(gl_BaseInstance).modelMatrix *
                GetDebugSkeleton(gl_BaseInstance).bones[gl_VertexIndex] *
                vec4(0.0, 0.0, 0.0, 1.0);
}
