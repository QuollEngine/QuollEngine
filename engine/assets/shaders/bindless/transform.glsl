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

#ifdef Bindless
#define getTransform(register, index)                                          \
  GetBindlessResourceFromPC(TransformData, register).items[index]
#endif
