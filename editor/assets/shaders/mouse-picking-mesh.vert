#version 460
#extension GL_EXT_scalar_block_layout : require

layout(location = 0) in vec3 inPosition;

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

void main() {
  mat4 modelMatrix = getMeshTransform(gl_InstanceIndex).modelMatrix;

  vec4 worldPosition =
      getCamera().viewProj * modelMatrix * vec4(inPosition, 1.0f);

  gl_Position = worldPosition;
  outEntity = uDrawParams.meshEntities.entities[gl_InstanceIndex];
}
