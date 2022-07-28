#pragma once

#include "liquidator/editor-scene/EditorManager.h"
#include "liquidator/editor-scene/EntityManager.h"

namespace liquidator {

/**
 * @brief Menu bar component
 */
class MenuBar {
public:
  /**
   * @brief Render menu bar
   *
   * @param editorManager Editor manager
   * @param entityManager Entity manager
   */
  void render(EditorManager &editorManager, EntityManager &entityManager);
};

} // namespace liquidator
