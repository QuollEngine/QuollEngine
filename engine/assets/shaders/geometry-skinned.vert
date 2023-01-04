#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec3 inColor;
layout(location = 4) in vec2 inTextureCoord0;
layout(location = 5) in vec2 inTextureCoord1;
layout(location = 6) in uvec4 inJoints;
layout(location = 7) in vec4 inWeights;

layout(location = 0) out vec3 outWorldPosition;
layout(location = 1) out vec2 outTextureCoord[2];
layout(location = 3) out vec3 outNormal;
layout(location = 4) out float outTangentHand;
layout(location = 5) out mat3 outTBN;

#include "bindless-base.glsl"

void main() {
  mat4 modelMatrix = getSkinnedMeshTransform(gl_InstanceIndex).modelMatrix;
  SkeletonItem item = getSkeleton(gl_InstanceIndex);

  mat4 skinMatrix = inWeights.x * item.joints[inJoints.x] +
                    inWeights.y * item.joints[inJoints.y] +
                    inWeights.z * item.joints[inJoints.z] +
                    inWeights.w * item.joints[inJoints.w];

  vec4 worldPosition = modelMatrix * skinMatrix * vec4(inPosition, 1.0f);

  mat3 m3ModelMatrix = mat3(modelMatrix);
  mat3 normalMatrix = transpose(inverse(m3ModelMatrix));

  vec3 normal = normalize(normalMatrix * inNormal);
  vec3 tangent = normalize(m3ModelMatrix * inTangent.xyz);
  vec3 bitangent = cross(normal, tangent) * inTangent.w;

  outWorldPosition = worldPosition.xyz;
  outNormal = normal;
  outTangentHand = inTangent.w;
  outTBN = mat3(tangent, bitangent, normal);
  outTextureCoord[0] = inTextureCoord0;
  outTextureCoord[1] = inTextureCoord1;
  gl_Position = getCamera().viewProj * worldPosition;
}
