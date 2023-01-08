#version 460
#extension GL_ARB_separate_shader_objects : enable

#include "bindless-editor.glsl"

void main() {
  gl_Position = getCamera().viewProj *
                getSkeletonTransform(gl_BaseInstance).modelMatrix *
                GetDebugSkeleton(gl_BaseInstance).bones[gl_VertexIndex] *
                vec4(0.0, 0.0, 0.0, 1.0);
}
