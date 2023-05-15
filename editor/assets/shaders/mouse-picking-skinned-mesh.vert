#version 460
#extension GL_EXT_scalar_block_layout : require

layout(location = 0) in vec3 inPosition;
layout(location = 6) in uvec4 inJoints;
layout(location = 7) in vec4 inWeights;

layout(location = 0) out uint outEntity;

#include "bindless-editor.glsl"

Buffer(16) EntitiesArray { uint entities[]; };

layout(set = 0, binding = 0) uniform DrawParams {
  Empty selectedEntity;

  Camera camera;

  TransformsArray spriteTransforms;
  EntitiesArray spriteEntities;

  TransformsArray meshTransforms;
  EntitiesArray meshEntities;

  TransformsArray skinnedMeshTransforms;
  EntitiesArray skinnedMeshEntities;
  SkeletonsArray skeletons;
}
uDrawParams;

const vec2 positions[4] =
    vec2[](vec2(-1, -1), vec2(+1, -1), vec2(-1, +1), vec2(+1, +1));

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
  outEntity = uDrawParams.skinnedMeshEntities.entities[gl_InstanceIndex];
}
