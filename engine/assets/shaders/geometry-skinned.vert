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
layout(location = 3) out mat3 outTBN;

#include "bindless/base.glsl"
#include "bindless/mesh.glsl"
#include "bindless/camera.glsl"

layout(set = 2, binding = 0) uniform DrawParameters {
  TransformsArray meshTransforms;
  TransformsArray skinnedMeshTransforms;
  SkeletonsArray skeletons;
  Camera camera;
  Empty scene;
  Empty directionalLights;
  Empty pointLights;
  Empty shadows;
}
uDrawParams;

void main() {
  mat4 worldMatrix = getSkinnedMeshTransform(gl_InstanceIndex).modelMatrix;
  SkeletonItem item = getSkeleton(gl_InstanceIndex);

  mat4 skinMatrix = inWeights.x * item.joints[inJoints.x] +
                    inWeights.y * item.joints[inJoints.y] +
                    inWeights.z * item.joints[inJoints.z] +
                    inWeights.w * item.joints[inJoints.w];

  mat4 modelMatrix = worldMatrix * skinMatrix;

  vec4 worldPosition = modelMatrix * vec4(inPosition, 1.0f);

  mat4 normalMatrix = transpose(inverse(modelMatrix));

  vec3 normal = normalize(vec3(normalMatrix * vec4(inNormal.xyz, 0.0)));
  vec3 tangent = normalize(vec3(modelMatrix * vec4(inTangent.xyz, 0.0)));
  vec3 bitangent = normalize(cross(normal, tangent));

  outWorldPosition = worldPosition.xyz;
  outTBN = mat3(tangent, bitangent, normal);
  outTextureCoord[0] = inTextureCoord0;
  outTextureCoord[1] = inTextureCoord1;
  gl_Position = getCamera().viewProj * worldPosition;
}
