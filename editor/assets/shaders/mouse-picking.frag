#version 460
#extension GL_EXT_scalar_block_layout : require

layout(location = 0) in flat uint outEntity;

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

#include "../../../engine/assets/shaders/bindless/base.glsl"

RegisterBuffer(scalar, writeonly, SelectedEntityData, { uint selectedEntity; });

void main() {
  GetBindlessResource(SelectedEntityData, uDrawParams.selectedEntity)
      .selectedEntity = outEntity;
}
