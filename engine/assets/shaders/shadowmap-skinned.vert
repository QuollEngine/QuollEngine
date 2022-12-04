#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shader_viewport_layer_array : enable

layout(location = 0) in vec3 inPosition;
layout(location = 6) in uvec4 inJoints;
layout(location = 7) in vec4 inWeights;

struct LightItem {
  /**
   * Light data
   */
  vec4 data;

  /**
   * Light color
   */
  vec4 color;

  /**
   * Light space matrix
   */
  mat4 lightMatrix;
};

layout(std140, set = 0, binding = 0) readonly buffer LightData {
  LightItem items[];
}
uLightData;

/**
 * @brief Single shadow data
 */
struct ShadowMapItem {
  /**
   * Shadow matrix generated from light
   */
  mat4 shadowMatrix;

  /**
   * Shadow data
   */
  vec4 shadowData;
};

layout(std140, set = 0, binding = 0) readonly buffer ShadowMapData {
  ShadowMapItem items[];
}
uShadowMaps;

/**
 * @brief Single object transforms
 */
struct ObjectItem {
  /**
   * Model matrix for object
   */
  mat4 modelMatrix;
};

/**
 * @brief Single skeleton transforms
 */
struct SkeletonItem {
  /**
   * Joint matrices of skeleton
   */
  mat4 joints[32];
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
pcShadowRef;

void main() {
  mat4 modelMatrix = uObjectData.items[gl_InstanceIndex].modelMatrix;
  SkeletonItem item = uSkeletonData.items[gl_InstanceIndex];

  mat4 skinMatrix = inWeights.x * item.joints[inJoints.x] +
                    inWeights.y * item.joints[inJoints.y] +
                    inWeights.z * item.joints[inJoints.z] +
                    inWeights.w * item.joints[inJoints.w];

  gl_Position = uShadowMaps.items[pcShadowRef.index.x].shadowMatrix *
                modelMatrix * skinMatrix * vec4(inPosition, 1.0);
  gl_Layer = pcShadowRef.index.x;
}
