#define BindlessDescriptorSet 0
#define BindlessStorageBinding 0
#define BindlessUniformBinding 1

// Ignore, used to identify bindless descriptor layout
// in pipelines
layout(set = BindlessDescriptorSet,
       binding = BindlessStorageBinding) readonly buffer DummyGlobalData {
  uint ignore[];
}
uGlobalBuffers[];

/**
 * @brief Single object transforms
 */
struct TransformItem {
  /**
   * Object model matrix
   */
  mat4 modelMatrix;
};

layout(set = BindlessDescriptorSet,
       binding = BindlessStorageBinding) readonly buffer TransformData {
  TransformItem items[];
}
uTransformsRegister[];

/**
 * @brief Single skeleton joints
 */
struct SkeletonItem {
  /**
   * Joints for skeleton
   */
  mat4 joints[32];
};

layout(set = BindlessDescriptorSet,
       binding = BindlessStorageBinding) readonly buffer SkeletonData {
  SkeletonItem items[];
}
uSkeletonsRegister[];

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

layout(set = BindlessDescriptorSet,
       binding = BindlessStorageBinding) readonly buffer GlyphData {
  GlyphItem items[];
}
uGlyphsRegister[];

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

layout(set = BindlessDescriptorSet,
       binding = BindlessStorageBinding) readonly buffer LightData {
  LightItem items[];
}
uLightsRegister[];

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

layout(set = BindlessDescriptorSet,
       binding = BindlessStorageBinding) buffer ShadowMapData {
  ShadowMapItem items[];
}
uShadowMapsRegister[];

layout(set = BindlessDescriptorSet,
       binding = BindlessUniformBinding) uniform DummyUniformData {
  uint ignore;
}
uGlobalUniforms[];

struct CameraData {
  mat4 proj;
  mat4 view;
  mat4 viewProj;
};

layout(set = BindlessDescriptorSet,
       binding = BindlessUniformBinding) uniform CameraUniform {
  CameraData camera;
}
uCameraRegister[];

struct SceneData {
  uvec4 data;

  uvec4 textures;
};

layout(set = BindlessDescriptorSet,
       binding = BindlessUniformBinding) uniform SceneUniform {
  SceneData scene;
}
uSceneRegister[];

struct BindlessBufferIndices {
  uint meshTransforms;
  uint skinnedMeshTransforms;
  uint skeletons;
  uint textTransforms;
  uint textGlyphs;
  uint lights;
  uint shadowMaps;

  // uniforms
  uint camera;
  uint scene;
};

const BindlessBufferIndices bufferIndices =
    BindlessBufferIndices(0, 1, 2, 3, 4, 5, 6, 0, 1);

#define getMeshTransform(index)                                                \
  uTransformsRegister[bufferIndices.meshTransforms].items[index]

#define getSkinnedMeshTransform(index)                                         \
  uTransformsRegister[bufferIndices.skinnedMeshTransforms].items[index]

#define getSkeleton(index)                                                     \
  uSkeletonsRegister[bufferIndices.skeletons].items[index]

#define getTextTransform(index)                                                \
  uTransformsRegister[bufferIndices.textTransforms].items[index]

#define getGlyph(index) uGlyphsRegister[bufferIndices.textGlyphs].items[index]

#define getLight(index) uLightsRegister[bufferIndices.lights].items[index]

#define getShadowMap(index)                                                    \
  uShadowMapsRegister[bufferIndices.shadowMaps].items[index]

#define getCamera() uCameraRegister[bufferIndices.camera].camera

#define getSceneData() uSceneRegister[bufferIndices.scene].scene
