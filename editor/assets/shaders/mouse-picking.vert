#version 460
#extension GL_EXT_scalar_block_layout : require

layout(location = 0) in vec3 inPosition;

layout(location = 0) out uint outEntity;

layout(set = 0, binding = 0) uniform CameraData {
  mat4 proj;
  mat4 view;
  mat4 viewProj;
}
uCameraData;

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

layout(scalar, set = 1, binding = 1) readonly buffer EntityData {
  uint entities[];
}
uEntityData;

void main() {
  mat4 modelMatrix = uObjectData.items[gl_BaseInstance].modelMatrix;

  vec4 worldPosition =
      uCameraData.viewProj * modelMatrix * vec4(inPosition, 1.0f);

  gl_Position = worldPosition;
  outEntity = uEntityData.entities[gl_BaseInstance];
}
