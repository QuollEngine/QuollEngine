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

RegisterBuffer(std430, readonly, DirectionalLightData,
               { DirectionalLightItem items[]; });

#define getDirectionalLight(index)                                             \
  GetBindlessResource(DirectionalLightData, uDrawParams.directionalLights)     \
      .items[index]

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

  /**
   * Shadow data
   */
  uvec4 shadowData;
};

RegisterBuffer(std430, readonly, PointLightData, { PointLightItem items[]; });

#define getPointLight(index)                                                   \
  GetBindlessResource(PointLightData, uDrawParams.pointLights).items[index]
