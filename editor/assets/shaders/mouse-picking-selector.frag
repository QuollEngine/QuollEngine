#version 460
#extension GL_ARB_gpu_shader_int64 : require
#extension GL_EXT_scalar_block_layout : require

layout(location = 0) in flat uint64_t outEntity;

#include "mouse-picking-base.glsl"

void main() { uDrawParams.selectedEntity.selectedEntity = outEntity; }
