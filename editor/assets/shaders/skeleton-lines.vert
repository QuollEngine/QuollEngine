#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform CameraData {
  mat4 proj;
  mat4 view;
  mat4 viewProj;
}
uCameraData;

struct ObjectItem {
  mat4 modelMatrix;
};

struct SkeletonItem {
  mat4 bones[64];
};

layout(std140, set = 0, binding = 1) readonly buffer ObjectData {
  ObjectItem items[];
}
uObjectData;

layout(std140, set = 0, binding = 2) readonly buffer SkeletonData {
  SkeletonItem items[];
}
uSkeletonData;

void main() {
  gl_Position = uCameraData.viewProj *
                uObjectData.items[gl_BaseInstance].modelMatrix *
                uSkeletonData.items[gl_BaseInstance].bones[gl_VertexIndex] *
                vec4(0.0, 0.0, 0.0, 1.0);
}
