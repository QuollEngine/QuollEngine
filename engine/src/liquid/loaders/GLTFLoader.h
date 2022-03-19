#pragma once

#include "liquid/entity/EntityContext.h"
#include "liquid/renderer/Renderer.h"
#include "liquid/animation/AnimationSystem.h"
#include "GLTFError.h"

namespace liquid {

class GLTFLoader {
  using Res = Errorable<SceneNode *, GLTFError>;

public:
  /**
   * @brief Creates loader with shaders and renderer
   *
   * @param entityContext Entity context
   * @param renderer Vulkan renderer
   * @param animationSystem Animation system
   * @param debug Enable debugging parameters
   */
  GLTFLoader(EntityContext &entityContext, Renderer &renderer,
             AnimationSystem &animationSystem, bool debug = false);

  /**
   * @brief Load GLTF from ASCII file
   *
   * @param filename File name
   * @return Scene node
   */
  Res loadFromFile(const String &filename);

private:
  EntityContext &mEntityContext;
  AnimationSystem &mAnimationSystem;
  Renderer &mRenderer;
  bool mDebug = false;

  SharedPtr<Material> mDefaultMaterial;
};

} // namespace liquid
