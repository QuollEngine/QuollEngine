#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shader_viewport_layer_array : enable

layout(location = 0) in vec3 inPosition;
layout(location = 6) in uvec4 inJoints;
layout(location = 7) in vec4 inWeights;

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

struct SkeletonItem {
  mat4 joints[16];
};

layout(std140, set = 1, binding = 0) readonly buffer ObjectData {
  ObjectItem items[];
}
uObjectData;

layout(std140, set = 1, binding = 1) readonly buffer SkeletonData {
  SkeletonItem items[];
}
uSkeletonData;

layout(push_constant) uniform PushConstants { ivec4 index; }
pcLightRef;

void main() {
  mat4 modelMatrix = uObjectData.items[gl_BaseInstance].modelMatrix;
  SkeletonItem item = uSkeletonData.items[gl_BaseInstance];

  mat4 skinMatrix = inWeights.x * item.joints[inJoints.x] +
                    inWeights.y * item.joints[inJoints.y] +
                    inWeights.z * item.joints[inJoints.z] +
                    inWeights.w * item.joints[inJoints.w];

  gl_Position = uLightData.items[pcLightRef.index.x].lightMatrix * modelMatrix *
                skinMatrix * vec4(inPosition, 1.0);
  gl_Layer = pcLightRef.index.x;
}
