#version 460

layout(location = 0) in vec3 inPosition;

layout(set = 0, binding = 0) uniform CameraData {
  mat4 proj;
  mat4 view;
  mat4 viewProj;
}
uCameraData;

layout(set = 0, binding = 1) uniform CollidableParams {
  mat4 worldTransform;
  uvec4 type;
  vec4 params;
}
uCollidableParams;

void main() {
  vec3 finalPosition = inPosition;

  if (uCollidableParams.type.x == 0) {
    // Box
    vec3 scale = vec3(uCollidableParams.params) * 2.0;
    finalPosition *= scale;
  } else if (uCollidableParams.type.x == 1) {
    // Sphere
    vec3 scale = vec3(uCollidableParams.params.x);
    finalPosition *= scale;
  } else if (uCollidableParams.type.x == 2) {
    // Capsule
    vec3 scale = vec3(uCollidableParams.params.x, uCollidableParams.params.y,
                      uCollidableParams.params.x);
    finalPosition *= scale;
  }

  gl_Position = uCameraData.viewProj * uCollidableParams.worldTransform *
                vec4(finalPosition, 1.0);
}
