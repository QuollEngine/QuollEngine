/**
 * @brief Directional light item
 */
struct DirectionalLightItem {
  /**
   * Light data
   */
  vec4 data;

  /**
   * Light color
   */
  vec4 color;

  /**
   * Shadow data
   */
  uvec4 shadowData;
};

Buffer(16) DirectionalLightsArray { DirectionalLightItem items[]; };

#define getDirectionalLight(index) uDrawParams.directionalLights.items[index]

/**
 * @brief Point light item
 */
struct PointLightItem {
  /**
   * Light data
   */
  vec4 data;

  /**
   * Light range
   */
  vec4 range;

  /**
   * Light color
   */
  vec4 color;
};

Buffer(16) PointLightsArray { PointLightItem items[]; };

#define getPointLight(index) uDrawParams.pointLights.items[index]
