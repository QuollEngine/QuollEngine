#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shader_viewport_layer_array : enable

layout(location = 0) in vec3 inPosition;

struct LightItem {
  vec4 data;
  vec4 color;
  mat4 lightMatrix;
};

layout(std140, set = 0, binding = 0) readonly buffer LightData {
  LightItem items[];
}
uLightData;

struct ObjectItem {
  mat4 modelMatrix;
};

layout(std140, set = 1, binding = 0) readonly buffer ObjectData {
  ObjectItem items[];
}
uObjectData;

layout(push_constant) uniform PushConstants { ivec4 index; }
pcLightRef;

void main() {
  mat4 modelMatrix = uObjectData.items[gl_BaseInstance].modelMatrix;

  gl_Position = uLightData.items[pcLightRef.index.x].lightMatrix * modelMatrix *
                vec4(inPosition, 1.0);
  gl_Layer = pcLightRef.index.x;
}
