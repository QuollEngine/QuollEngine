#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 outTexCoord;

layout(set = 0, binding = 0) uniform CameraData {
  mat4 proj;
  mat4 view;
  mat4 viewProj;
}
uCameraData;

void main() {
  mat4 viewWithoutTranslation =
      mat4(uCameraData.view[0], uCameraData.view[1], uCameraData.view[2],
           vec4(0.0, 0.0, 0.0, 1.0));
  vec4 position =
      uCameraData.proj * viewWithoutTranslation * vec4(inPosition.xyz, 1.0);

  gl_Position = position.xyww;
  outTexCoord = inPosition;
}
