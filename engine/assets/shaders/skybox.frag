#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec3 inTexCoord;
layout(location = 0) out vec4 outColor;

#include "bindless/base.glsl"

layout(set = 0, binding = 0) uniform textureCube uGlobalTextures[];
layout(set = 0, binding = 1) uniform sampler uGlobalSamplers[];
layout(set = 0, binding = 2) writeonly uniform image2D uGlobalImages[];

/**
 * @brief Skybox
 */
Buffer(16) Skybox {
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

layout(set = 1, binding = 0) uniform DrawParams {
  Empty camera;
  Skybox skybox;
  uint defaultSampler;
}
uDrawParams;

#define getSkybox() uDrawParams.skybox

void main() {
  if (getSkybox().data.x > 0) {
    vec3 color =
        textureLod(samplerCube(uGlobalTextures[getSkybox().data.x],
                               uGlobalSamplers[uDrawParams.defaultSampler]),
                   inTexCoord, 0)
            .xyz;

    outColor = vec4(color, 1.0);
  } else {
    outColor = vec4(getSkybox().color.xyz, 1.0);
  }
}
