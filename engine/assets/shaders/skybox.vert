#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 outTexCoord;

#include "bindless/base.glsl"
#include "bindless/camera.glsl"

layout(set = 1, binding = 0) uniform DrawParameters {
  Camera camera;
  Empty skybox;
  uint defaultSampler;
}
uDrawParams;

void main() {
  mat4 viewWithoutTranslation =
      mat4(getCamera().view[0], getCamera().view[1], getCamera().view[2],
           vec4(0.0, 0.0, 0.0, 1.0));
  vec4 position =
      getCamera().proj * viewWithoutTranslation * vec4(inPosition.xyz, 1.0);

  gl_Position = position.xyww;
  outTexCoord = inPosition;
}
