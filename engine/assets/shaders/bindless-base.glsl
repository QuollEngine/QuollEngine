#include "bindless/base.glsl"
#include "bindless/transform.glsl"
#include "bindless/camera.glsl"

/**
 * @brief Single skeleton joints
 */
struct SkeletonItem {
  /**
   * Joints for skeleton
   */
  mat4 joints[32];
};

RegisterBuffer(std430, readonly, SkeletonData, { SkeletonItem items[]; });

/**
 * @brief Single glyph data
 */
struct GlyphItem {
  /**
   * Glyph atlas bounds
   */
  vec4 bounds;

  /**
   * Glyph quad bounds
   */
  vec4 planeBounds;
};

RegisterBuffer(std430, readonly, GlyphData, { GlyphItem items[]; });

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

struct SceneData {
  uvec4 data;

  uvec4 textures;
};

RegisterUniform(SceneUniform, { SceneData scene; });

#define getMeshTransform(index) getTransform(0, index)

#define getSkinnedMeshTransform(index) getTransform(1, index)

#define getSkeleton(index)                                                     \
  GetBindlessResourceFromPC(SkeletonData, 2).items[index]

#define getTextTransform(index) getTransform(3, index)

#define getGlyph(index) GetBindlessResourceFromPC(GlyphData, 4).items[index]

#define getLight(index) GetBindlessResourceFromPC(LightData, 5).items[index]

#define getShadowMap(index)                                                    \
  GetBindlessResourceFromPC(ShadowMapData, 6).items[index]

#define getCamera() getCameraFromReg(7)

#define getSceneData() GetBindlessResourceFromPC(SceneUniform, 8).scene
