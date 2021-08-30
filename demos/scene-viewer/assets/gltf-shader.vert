#version 450

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec4 vTangent;
layout(location = 3) in vec3 vColor;
layout(location = 4) in vec2 vTextureCoord0;
layout(location = 5) in vec2 vTextureCoord1;

layout(location = 0) out vec4 outModelPosition;
layout(location = 1) out vec3 outWorldPosition;
layout(location = 2) out vec3 outColor;
layout(location = 3) out vec2 outTextureCoord[2];
layout(location = 5) out vec3 outNormal;
layout(location = 6) out float outTangentHand;
layout(location = 7) out mat3 outTBN;
layout(location = 10) out mat4 outModelMatrix;

layout(set = 0, binding = 0) uniform CameraBuffer {
  mat4 proj;
  mat4 view;
  mat4 viewproj;
}
cameraData;

layout(push_constant) uniform constants { mat4 modelMatrix; }
pushConstants;

void main() {
  vec4 worldPosition =
      cameraData.viewproj * pushConstants.modelMatrix * vec4(vPosition, 1.0f);

  mat3 m3ModelMatrix = mat3(pushConstants.modelMatrix);
  mat3 normalMatrix = transpose(inverse(m3ModelMatrix));

  vec3 normal = normalize(normalMatrix * vNormal);
  vec3 tangent = normalize(m3ModelMatrix * vTangent.xyz);
  vec3 bitangent = cross(normal, tangent) * vTangent.w;

  outModelMatrix = pushConstants.modelMatrix;
  outModelPosition = vec4(vPosition, 1.0f);
  outWorldPosition = worldPosition.xyz;
  outNormal = normal;
  outTangentHand = vTangent.w;
  outTBN = mat3(tangent, bitangent, normal);
  outTextureCoord[0] = vTextureCoord0;
  outTextureCoord[1] = vTextureCoord1;
  outColor = vColor;
  gl_Position = worldPosition;
}
