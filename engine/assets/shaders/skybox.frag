#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec3 inTexCoord;

layout(location = 0) out vec4 outColor;

layout(set = 2, binding = 0) uniform DrawParams {
  uint camera;
  uint skybox;
  uint pad0;
  uint pad1;
}
uDrawParams;

#include "bindless/base.glsl"

layout(set = 1, binding = 0) uniform samplerCube uGlobalTextures[];

/**
 * @brief Skybox data
 */
struct SkyboxData {
  /**
   * Data
   *
   * First parameter is texture handle
   */
  uvec4 data;

  /**
   * Skybox color
   */
  vec4 color;
};

RegisterUniform(SkyboxUniform, { SkyboxData skybox; });

#define getSkyboxData()                                                        \
  GetBindlessResource(SkyboxUniform, uDrawParams.skybox).skybox

void main() {
  if (getSkyboxData().data.x > 0) {
    vec3 color =
        textureLod(uGlobalTextures[getSkyboxData().data.x], inTexCoord, 0).xyz;

    outColor = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
  } else {
    outColor = vec4(getSkyboxData().color.xyz, 1.0);
  }
}
