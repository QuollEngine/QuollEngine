#version 460
#extension GL_EXT_scalar_block_layout : require

layout(location = 0) in flat uint outEntity;

#include "bindless-editor.glsl"

Buffer(16) SelectedEntity { uint selectedEntity; };

layout(set = 0, binding = 0) uniform DrawParams {
  Empty meshTransforms;
  Empty skinnedMeshTransforms;
  Empty skeletons;
  Empty camera;
  Empty entities;
  SelectedEntity selectedEntity;
  Empty pad0;
  Empty pad1;
}
uDrawParams;

void main() { uDrawParams.selectedEntity.selectedEntity = outEntity; }
