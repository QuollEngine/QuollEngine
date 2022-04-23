#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shader_viewport_layer_array : enable

layout(location = 0) in vec3 inPosition;

layout(std140, set = 0, binding = 0) uniform MaterialData {
  mat4 lightMatrix;
  int lightIndex[1];
}
uMaterialData;

struct ObjectItem {
  mat4 modelMatrix;
};

layout(std140, set = 1, binding = 0) readonly buffer ObjectData {
  ObjectItem items[];
}
uObjectData;

void main() {
  mat4 modelMatrix = uObjectData.items[gl_BaseInstance].modelMatrix;

  gl_Position = uMaterialData.lightMatrix * modelMatrix * vec4(inPosition, 1.0);
  gl_Layer = uMaterialData.lightIndex[0];
}
