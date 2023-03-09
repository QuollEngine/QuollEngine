#pragma once

#include "liquidator/editor-scene/EditorManager.h"
#include "liquidator/editor-scene/EntityManager.h"

namespace liquid::editor {

/**
 * @brief Menu bar component
 */
class MenuBar {
public:
  /**
   * @brief Render menu bar
   *
   * @param state Workspace state
   * @param editorManager Editor manager
   * @param entityManager Entity manager
   */
  static void render(WorkspaceState &state, EditorManager &editorManager,
                     EntityManager &entityManager);
};

} // namespace liquid::editor
