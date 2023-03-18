struct SceneData {
  uvec4 data;

  uvec4 textures;

  vec4 color;
};

RegisterUniform(SceneUniform, { SceneData scene; });

#define getScene() GetBindlessResource(SceneUniform, uDrawParams.scene).scene
