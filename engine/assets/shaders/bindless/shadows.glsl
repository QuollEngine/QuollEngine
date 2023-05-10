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

Buffer(64) ShadowMapsArray { ShadowMapItem items[]; };

#define getShadowMap(index) uDrawParams.shadows.items[index]
