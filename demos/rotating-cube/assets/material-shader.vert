#version 450

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 3) in vec3 vColor;

layout(location = 0) out vec3 outColor;
layout(location = 2) out float albedo;
layout(location = 3) out float specular;

layout(set = 0, binding = 0) uniform CameraBuffer {
  mat4 proj;
  mat4 view;
  mat4 viewproj;
}
cameraData;

layout(push_constant) uniform constants { mat4 modelMatrix; }
pushConstants;

vec3 lightPosition = vec3(2.0, 1.0, 0.0);

layout(set = 1, binding = 0) uniform MaterialBuffer {
  vec4 diffuse;
  float shininess;
  vec4 specular;
}
materialData;

void main() {
  mat4 transform = cameraData.viewproj * pushConstants.modelMatrix;

  gl_Position = transform * vec4(vPosition, 1.0f);

  mat3 transposedNormalMatrix = transpose(inverse(mat3(transform)));
  vec3 normal = normalize(transposedNormalMatrix * vNormal);

  vec3 lightDirection = normalize(lightPosition - vec3(gl_Position));
  vec3 halfDir = normalize(lightDirection - normalize(vec3(gl_Position)));

  albedo = dot(normal, lightDirection);
  specular = pow(max(dot(normal, halfDir), 0.0), materialData.shininess);

  outColor = vColor;
}
