#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shader_viewport_layer_array : enable

layout(location = 0) in vec3 inPosition;

#include "bindless-base.glsl"

void main() {
  mat4 modelMatrix = getMeshTransform(gl_InstanceIndex).modelMatrix;

  gl_Position = getShadowMap(pcDrawParameters.index9).shadowMatrix *
                modelMatrix * vec4(inPosition, 1.0);
  gl_Layer = int(pcDrawParameters.index9);
}
