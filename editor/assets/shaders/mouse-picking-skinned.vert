#version 460
#extension GL_EXT_scalar_block_layout : require

layout(location = 0) in vec3 inPosition;
layout(location = 6) in uvec4 inJoints;
layout(location = 7) in vec4 inWeights;

layout(location = 0) out uint outEntity;

#include "../../../engine/assets/shaders/bindless/base.glsl"
#include "../../../engine/assets/shaders/bindless/mesh.glsl"
#include "../../../engine/assets/shaders/bindless/camera.glsl"

RegisterBuffer(scalar, readonly, EntityData, { uint entities[]; });

layout(set = 1, binding = 0) uniform DrawParams {
  uint meshTransforms;
  uint skinnedMeshTransforms;
  uint skeletons;
  uint camera;
  uint entities;
  uint selectedEntity;
  uint pad0;
  uint pad1;
}
uDrawParams;

void main() {
  mat4 modelMatrix = getSkinnedMeshTransform(gl_InstanceIndex).modelMatrix;

  SkeletonItem item = getSkeleton(gl_InstanceIndex);

  mat4 skinMatrix = inWeights.x * item.joints[inJoints.x] +
                    inWeights.y * item.joints[inJoints.y] +
                    inWeights.z * item.joints[inJoints.z] +
                    inWeights.w * item.joints[inJoints.w];

  vec4 worldPosition =
      getCamera().viewProj * modelMatrix * skinMatrix * vec4(inPosition, 1.0f);

  gl_Position = worldPosition;
  outEntity = GetBindlessResource(EntityData, uDrawParams.entities)
                  .entities[gl_InstanceIndex];
}
