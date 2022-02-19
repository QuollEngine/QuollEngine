#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform CameraData {
  mat4 proj;
  mat4 view;
  mat4 viewProj;
}
uCameraData;

layout(set = 1, binding = 0) uniform SkeletonData { mat4 joints[44]; }
uSkeletonData;

layout(push_constant) uniform TransformConstant { mat4 modelMatrix; }
pcTransform;

void main() {
  gl_Position = uCameraData.viewProj * pcTransform.modelMatrix *
                uSkeletonData.joints[gl_VertexIndex] * vec4(0.0, 0.0, 0.0, 1.0);
}
