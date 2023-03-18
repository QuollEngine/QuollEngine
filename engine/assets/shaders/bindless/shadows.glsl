/**
 * @brief Single shadow data
 */
struct ShadowMapItem {
  /**
   * Shadow matrix generated from light
   */
  mat4 shadowMatrix;

  /**
   * Shadow data
   */
  vec4 shadowData;
};

RegisterBuffer(std430, readonly, ShadowMapData, { ShadowMapItem items[]; });

#define getShadowMap(index)                                                    \
  GetBindlessResource(ShadowMapData, uDrawParams.shadows).items[index]
