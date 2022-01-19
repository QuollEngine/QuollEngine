#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 3) in vec3 inColor;

layout(location = 0) out vec3 outColor;

layout(push_constant) uniform TransformConstant { mat4 modelMatrix; }
pcTransform;

void main() {
  gl_Position = pcTransform.modelMatrix * vec4(inPosition, 1.0f);
  outColor = inColor;
}
