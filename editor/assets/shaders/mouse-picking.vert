#version 460
#extension GL_EXT_scalar_block_layout : require

layout(location = 0) in vec3 inPosition;

layout(location = 0) out uint outEntity;

#include "../../../engine/assets/shaders/bindless-base.glsl"

RegisterBuffer(scalar, readonly, EntityData, { uint entities[]; });

void main() {
  mat4 modelMatrix = getMeshTransform(gl_InstanceIndex).modelMatrix;

  vec4 worldPosition =
      getCamera().viewProj * modelMatrix * vec4(inPosition, 1.0f);

  gl_Position = worldPosition;
  outEntity = GetBindlessResource(EntityData, pcDrawParameters.index10)
                  .entities[gl_InstanceIndex];
}
