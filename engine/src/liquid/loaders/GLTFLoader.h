#pragma once

#include "liquid/entity/EntityContext.h"
#include "liquid/renderer/Renderer.h"
#include "GLTFError.h"

namespace liquid {

/**
 * @deprecated This component should not be used anymore
 */
class GLTFLoader {
  using Res = Errorable<SceneNode *, GLTFError>;

public:
  /**
   * @brief Creates loader with shaders and renderer
   *
   * @param entityContext Entity context
   * @param renderer Vulkan renderer
   * @param debug Enable debugging parameters
   */
  GLTFLoader(EntityContext &entityContext, Renderer &renderer,
             bool debug = false);

  /**
   * @brief Load GLTF from ASCII file
   *
   * @param filename File name
   * @return Scene node
   */
  Res loadFromFile(const String &filename);

private:
  EntityContext &mEntityContext;
  Renderer &mRenderer;
  bool mDebug = false;

  SharedPtr<Material> mDefaultMaterial;
};

} // namespace liquid
