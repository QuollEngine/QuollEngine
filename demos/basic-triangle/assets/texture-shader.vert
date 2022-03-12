#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 3) in vec3 inColor;
layout(location = 4) in vec2 inTextureCoord;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTextureCoord;

layout(push_constant) uniform TransformConstant { mat4 modelMatrix; }
pcTransform;

void main() {
  gl_Position = vec4(inPosition, 1.0f);
  outColor = inColor;
  outTextureCoord = inTextureCoord;
}
