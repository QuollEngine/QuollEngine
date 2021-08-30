#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vPosition;

layout(location = 0) out vec3 texCoord;

layout(set = 0, binding = 0) uniform CameraBuffer {
  mat4 proj;
  mat4 view;
  mat4 viewproj;
}
cameraData;

layout(push_constant) uniform constants { mat4 modelMatrix; }
pushConstants;

void main() {
  mat4 viewWithoutTranslation =
      mat4(cameraData.view[0], cameraData.view[1], cameraData.view[2],
           vec4(0.0, 0.0, 0.0, 1.0));
  vec4 position =
      cameraData.proj * viewWithoutTranslation * vec4(vPosition.xyz, 1.0);
  gl_Position = position.xyww;
  texCoord = vPosition;
}
