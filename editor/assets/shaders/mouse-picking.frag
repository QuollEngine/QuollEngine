#version 460
#extension GL_EXT_scalar_block_layout : require

layout(location = 0) in flat uint outEntity;

#include "../../../engine/assets/shaders/bindless-base.glsl"

RegisterBuffer(scalar, writeonly, SelectedEntityData, { uint selectedEntity; });

void main() {
  GetBindlessResource(SelectedEntityData, pcDrawParameters.index9)
      .selectedEntity = outEntity;
}
