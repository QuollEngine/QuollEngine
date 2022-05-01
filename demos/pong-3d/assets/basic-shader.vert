#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 3) in vec3 inColor;

layout(location = 0) out vec3 outColor;

layout(set = 0, binding = 0) uniform CameraData {
  mat4 proj;
  mat4 view;
  mat4 viewProj;
}
uCameraData;

struct ObjectItem {
  mat4 modelMatrix;
};

layout(std140, set = 1, binding = 0) readonly buffer ObjectData {
  ObjectItem items[];
}
uObjectData;

void main() {
  mat4 modelMatrix = uObjectData.items[gl_BaseInstance].modelMatrix;
  gl_Position = uCameraData.viewProj * modelMatrix * vec4(inPosition, 1.0f);
  outColor = inColor;
}
