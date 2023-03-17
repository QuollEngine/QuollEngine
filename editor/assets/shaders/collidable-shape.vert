#version 460

layout(location = 0) in vec3 inPosition;

#include "bindless-editor.glsl"

RegisterUniform(CollidableParamsUniform, {
  mat4 worldTransform;
  uvec4 type;
  vec4 params;
});
#define GetCollidableParams()                                                  \
  GetBindlessResource(CollidableParamsUniform, uDrawParams.collidableParams)

layout(set = 1, binding = 0) uniform DrawParameters {
  uint gizmoTransforms;
  uint skeletonTransforms;
  uint debugSkeletons;
  uint collidableParams;
  uint camera;
  uint gridData;
}
uDrawParams;

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

  gl_Position = getCamera().viewProj * GetCollidableParams().worldTransform *
                vec4(finalPosition, 1.0);
}
