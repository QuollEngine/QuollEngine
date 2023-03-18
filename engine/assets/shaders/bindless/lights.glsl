/**
 * @brief Single light data
 */
struct LightItem {
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

RegisterBuffer(std430, readonly, LightData, { LightItem items[]; });

#define getLight(index)                                                        \
  GetBindlessResource(LightData, uDrawParams.lights).items[index]
