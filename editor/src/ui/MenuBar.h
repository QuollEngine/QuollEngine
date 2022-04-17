#pragma once

#include "liquid/loaders/GLTFLoader.h"
#include "liquid/scene/Scene.h"

#include "../editor-scene/SceneManager.h"

namespace liquidator {

class MenuBar {
public:
  /**
   * @brief Render menu bar
   *
   * @param sceneManager Scene manager
   */
  void render(SceneManager &sceneManager);

private:
  /**
   * @brief Handle new scene item click
   *
   * @param sceneManager Scene manager
   */
  void handleNewScene(SceneManager &sceneManager);
};

} // namespace liquidator
