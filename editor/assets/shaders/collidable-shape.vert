#version 460

layout(location = 0) in vec3 inPosition;

#include "bindless-editor.glsl"

Buffer(64) CollidableParams {
  mat4 worldTransform;
  uvec4 type;
  vec4 center;
  vec4 params;
};

layout(set = 0, binding = 0) uniform DrawParameters {
  Empty gizmoTransforms;
  Empty skeletonTransforms;
  Empty debugSkeletons;
  CollidableParams collidableParams;
  Camera camera;
  Empty gridData;
  Empty pad0;
  Empty pad1;
}
uDrawParams;

#define GetCollidableParams() uDrawParams.collidableParams

void main() {
  vec3 finalPosition = inPosition;

  if (GetCollidableParams().type.x == 0) {
    // Box
    vec3 scale = vec3(GetCollidableParams().params) * 2.0;
    finalPosition *= scale;
  } else if (GetCollidableParams().type.x == 1) {
    // Sphere
    vec3 scale = vec3(GetCollidableParams().params.x);
    finalPosition *= scale;
  } else if (GetCollidableParams().type.x == 2) {
    // Capsule
    vec3 scale =
        vec3(GetCollidableParams().params.x, GetCollidableParams().params.y,
             GetCollidableParams().params.x);
    finalPosition *= scale;
  }

  finalPosition += GetCollidableParams().center.xyz;

  gl_Position = getCamera().viewProj * GetCollidableParams().worldTransform *
                vec4(finalPosition, 1.0);
}
