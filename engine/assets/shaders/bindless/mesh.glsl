#include "transform.glsl"
#include "skeleton.glsl"

#define getMeshTransform(index) uDrawParams.meshTransforms.items[index]

#define getSkinnedMeshTransform(index)                                         \
  uDrawParams.skinnedMeshTransforms.items[index]
