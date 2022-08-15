#version 460
#extension GL_EXT_scalar_block_layout : require

layout(location = 0) in vec3 inPosition;
layout(location = 6) in uvec4 inJoints;
layout(location = 7) in vec4 inWeights;

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

struct SkeletonItem {
  /**
   * Joints for skeleton
   */
  mat4 joints[32];
};

layout(std140, set = 1, binding = 1) readonly buffer SkeletonData {
  SkeletonItem items[];
}
uSkeletonData;

layout(scalar, set = 1, binding = 2) readonly buffer EntityData {
  uint entities[];
}
uEntityData;

void main() {
  mat4 modelMatrix = uObjectData.items[gl_BaseInstance].modelMatrix;

  SkeletonItem item = uSkeletonData.items[gl_BaseInstance];

  mat4 skinMatrix = inWeights.x * item.joints[inJoints.x] +
                    inWeights.y * item.joints[inJoints.y] +
                    inWeights.z * item.joints[inJoints.z] +
                    inWeights.w * item.joints[inJoints.w];

  vec4 worldPosition =
      uCameraData.viewProj * modelMatrix * skinMatrix * vec4(inPosition, 1.0f);

  gl_Position = worldPosition;
  outEntity = uEntityData.entities[gl_BaseInstance];
}
