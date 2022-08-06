#version 460
#extension GL_ARB_separate_shader_objects : enable

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
   * Model matrix for object
   */
  mat4 modelMatrix;
};

/**
 * @brief Single debug skeleton transforms
 */
struct DebugSkeletonItem {
  /**
   * Bone matrices for skeleton
   */
  mat4 bones[64];
};

layout(std140, set = 0, binding = 1) readonly buffer ObjectData {
  ObjectItem items[];
}
uObjectData;

layout(std140, set = 0, binding = 2) readonly buffer SkeletonData {
  DebugSkeletonItem items[];
}
uSkeletonData;

void main() {
  gl_Position = uCameraData.viewProj *
                uObjectData.items[gl_BaseInstance].modelMatrix *
                uSkeletonData.items[gl_BaseInstance].bones[gl_VertexIndex] *
                vec4(0.0, 0.0, 0.0, 1.0);
}
