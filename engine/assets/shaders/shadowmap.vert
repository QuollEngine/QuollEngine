#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shader_viewport_layer_array : enable

layout(location = 0) in vec3 inPosition;

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
   * Object model matrix
   */
  mat4 modelMatrix;
};

layout(std140, set = 1, binding = 0) readonly buffer ObjectData {
  ObjectItem items[];
}
uObjectData;

layout(push_constant) uniform PushConstants { ivec4 index; }
pcShadowRef;

void main() {
  mat4 modelMatrix = uObjectData.items[gl_InstanceIndex].modelMatrix;

  gl_Position = uShadowMaps.items[pcShadowRef.index.x].shadowMatrix *
                modelMatrix * vec4(inPosition, 1.0);
  gl_Layer = pcShadowRef.index.x;
}
