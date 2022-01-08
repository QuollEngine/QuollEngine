#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 3) in vec3 inColor;

layout(location = 0) out vec3 outColor;

layout(set = 0, binding = 0) uniform CameraData {
  mat4 proj;
  mat4 view;
  mat4 viewProj;
}
uCameraData;

layout(push_constant) uniform TransformConstant { mat4 modelMatrix; }
pcTransform;

void main() {
  gl_Position =
      uCameraData.viewProj * pcTransform.modelMatrix * vec4(inPosition, 1.0f);
  outColor = inColor;
}
