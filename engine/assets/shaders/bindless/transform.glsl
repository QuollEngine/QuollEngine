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

Buffer(64) TransformsArray { TransformItem items[]; };

#endif
