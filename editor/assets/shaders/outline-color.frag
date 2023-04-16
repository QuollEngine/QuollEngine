#version 460

layout(location = 0) out vec4 outColor;

/**
 * @brief Push constant outlines
 */
layout(push_constant) uniform PushConstants {
  vec4 color;
  vec4 scale;
  vec4 index;
}
pcOutline;

void main() { outColor = pcOutline.color; }
