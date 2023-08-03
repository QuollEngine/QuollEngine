#version 460
#extension GL_EXT_scalar_block_layout : require

layout(location = 0) in flat uint outEntity;

#include "mouse-picking-base.glsl"

void main() { uDrawParams.selectedEntity.selectedEntity = outEntity; }
