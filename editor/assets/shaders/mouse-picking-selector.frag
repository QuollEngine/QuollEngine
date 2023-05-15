#version 460
#extension GL_EXT_scalar_block_layout : require

layout(location = 0) in flat uint outEntity;

#include "bindless-editor.glsl"

Buffer(16) SelectedEntity { uint selectedEntity; };

layout(set = 0, binding = 0) uniform DrawParams {
  SelectedEntity selectedEntity;

  Empty camera;
  Empty spriteTransforms;
  Empty spriteEntities;
  Empty meshTransforms;
  Empty meshEntities;
  Empty skinnedMeshTransforms;
  Empty skinnedMeshEntities;
  Empty skeletons;
}
uDrawParams;

void main() { uDrawParams.selectedEntity.selectedEntity = outEntity; }
