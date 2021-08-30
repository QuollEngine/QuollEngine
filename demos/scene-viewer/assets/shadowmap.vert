#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shader_viewport_layer_array : enable

layout(location = 0) in vec3 vPosition;

layout(std140, set = 1, binding = 0) uniform MaterialData {
  mat4 lightMatrix;
  int lightIndex[1];
}
materialData;

layout(push_constant) uniform constants { mat4 modelMatrix; }
pushConstants;

void main() {
  gl_Position = materialData.lightMatrix * pushConstants.modelMatrix *
                vec4(vPosition, 1.0);
  gl_Layer = materialData.lightIndex[0];
}
