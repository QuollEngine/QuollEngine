#pragma once

#include "../editor-scene/SceneManager.h"
#include "../editor-scene/EntityManager.h"

namespace liquidator {

/**
 * @brief Menu bar component
 */
class MenuBar {
public:
  /**
   * @brief Render menu bar
   *
   * @param sceneManager Scene manager
   * @param entityManager Entity manager
   */
  void render(SceneManager &sceneManager, EntityManager &entityManager);
};

} // namespace liquidator
