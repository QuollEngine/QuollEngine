#version 450 core

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec4 inColor;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec2 outUV;

layout(push_constant) uniform TransformConstant { mat4 uiTransform; }
pcTransform;

void main() {
  outColor = inColor;
  outUV = inUV;
  gl_Position = pcTransform.uiTransform * vec4(inPosition, 0.0, 1.0);
}
