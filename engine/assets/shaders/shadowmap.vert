#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shader_viewport_layer_array : enable

layout(location = 0) in vec3 inPosition;

layout(std140, set = 0, binding = 0) uniform MaterialData {
  mat4 lightMatrix;
  int lightIndex[1];
}
uMaterialData;

layout(push_constant) uniform TransfromConstant { mat4 modelMatrix; }
pcTransform;

void main() {
  gl_Position = uMaterialData.lightMatrix * pcTransform.modelMatrix *
                vec4(inPosition, 1.0);
  gl_Layer = uMaterialData.lightIndex[0];
}
