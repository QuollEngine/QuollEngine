#version 460

layout(location = 0) out vec2 outTexCoord;

layout(set = 0, binding = 0) uniform CameraData {
  mat4 proj;
  mat4 view;
  mat4 viewProj;
}
uCameraData;

/**
 * @brief Single object transforms
 */
struct ObjectItem {
  /**
   * Model matrix for object
   */
  mat4 modelMatrix;
};

layout(std140, set = 0, binding = 1) readonly buffer ObjectData {
  ObjectItem items[];
}
uObjectData;

const vec2 positions[4] =
    vec2[](vec2(-1, -1), vec2(+1, -1), vec2(-1, +1), vec2(+1, +1));
const vec2 texCoords[4] =
    vec2[](vec2(0, 0), vec2(1, 0), vec2(0, 1), vec2(1, 1));

void main() {
  outTexCoord = texCoords[gl_VertexIndex];
  gl_Position = uCameraData.viewProj *
                uObjectData.items[gl_InstanceIndex].modelMatrix *
                vec4(positions[gl_VertexIndex], 0.0, 1.0);
}
