#version 460

layout(location = 0) out vec2 outTexCoord;

#include "bindless-editor.glsl"

layout(set = 1, binding = 0) uniform DrawParameters {
  TransformsArray gizmoTransforms;
  Empty skeletonTransforms;
  Empty debugSkeletons;
  Empty collidbaleParams;
  Camera camera;
  Empty gridData;
  Empty pad0;
  Empty pad1;
}
uDrawParams;

#define getGizmoTransform(index) uDrawParams.gizmoTransforms.items[index]

const vec2 positions[4] =
    vec2[](vec2(-1, -1), vec2(+1, -1), vec2(-1, +1), vec2(+1, +1));
const vec2 texCoords[4] =
    vec2[](vec2(0, 0), vec2(1, 0), vec2(0, 1), vec2(1, 1));

void main() {
  outTexCoord = texCoords[gl_VertexIndex];
  gl_Position = getCamera().viewProj *
                getGizmoTransform(gl_InstanceIndex).modelMatrix *
                vec4(positions[gl_VertexIndex], 0.0, 1.0);
}
