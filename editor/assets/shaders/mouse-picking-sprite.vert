#version 460
#extension GL_EXT_scalar_block_layout : require

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
  mat4 modelMatrix =
      uDrawParams.spriteTransforms.items[gl_InstanceIndex].modelMatrix;

  vec4 worldPosition = getCamera().viewProj * modelMatrix *
                       vec4(positions[gl_VertexIndex], 0.0, 1.0);

  gl_Position = worldPosition;
  outEntity = uDrawParams.spriteEntities.entities[gl_InstanceIndex];
}
