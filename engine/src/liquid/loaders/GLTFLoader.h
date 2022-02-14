#pragma once

#include "liquid/entity/EntityContext.h"
#include "liquid/renderer/vulkan/VulkanRenderer.h"
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
   */
  GLTFLoader(EntityContext &entityContext, VulkanRenderer *renderer,
             AnimationSystem &animationSystem);

  /**
   * @brief Load GLTF from ASCII file
   *
   * @param filename File name
   * @return Scene node
   */
  Res loadFromFile(const String &filename);

private:
  EntityContext &entityContext;
  AnimationSystem &animationSystem;
  VulkanRenderer *renderer = nullptr;

  SharedPtr<Material> defaultMaterial;
};

} // namespace liquid
