#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shader_viewport_layer_array : enable

layout(location = 0) in vec3 inPosition;
layout(location = 6) in uvec4 inJoints;
layout(location = 7) in vec4 inWeights;

layout(std140, set = 0, binding = 0) uniform MaterialData {
  mat4 lightMatrix;
  int lightIndex[1];
}
uMaterialData;

layout(set = 1, binding = 0) uniform SkeletonData { mat4 joints[16]; }
uSkeletonData;

layout(push_constant) uniform TransfromConstant { mat4 modelMatrix; }
pcTransform;

void main() {

  mat4 skinMatrix = inWeights.x * uSkeletonData.joints[inJoints.x] +
                    inWeights.y * uSkeletonData.joints[inJoints.y] +
                    inWeights.z * uSkeletonData.joints[inJoints.z] +
                    inWeights.w * uSkeletonData.joints[inJoints.w];

  gl_Position = uMaterialData.lightMatrix * pcTransform.modelMatrix *
                skinMatrix * vec4(inPosition, 1.0);
  gl_Layer = uMaterialData.lightIndex[0];
}
