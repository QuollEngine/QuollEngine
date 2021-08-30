#version 450

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vColor;
layout(location = 4) in vec2 vTextureCoord;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTextureCoord;

layout(set = 0, binding = 0) uniform CameraBuffer {
  mat4 proj;
  mat4 view;
  mat4 viewproj;
}
cameraData;

layout(push_constant) uniform constants { mat4 modelMatrix; }
pushConstants;

void main() {
  gl_Position =
      cameraData.viewproj * pushConstants.modelMatrix * vec4(vPosition, 1.0f);
  outColor = vColor;
  outTextureCoord = vTextureCoord;
}