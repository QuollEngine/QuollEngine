#ifndef TRANSFORM_GLSL
#define TRANSFORM_GLSL

/**
 * @brief Single object transforms
 */
struct TransformItem {
  /**
   * Object model matrix
   */
  mat4 modelMatrix;
};

RegisterBuffer(std430, readonly, TransformData, { TransformItem items[]; });

#endif
