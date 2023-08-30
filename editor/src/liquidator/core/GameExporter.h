#pragma once

#include "liquidator/project/Project.h"

namespace quoll::editor {

/**
 * @brief Game exporter
 */
class GameExporter {
public:
  /**
   * @brief Export project as a game
   *
   * @param project Project
   * @param destination Destination path
   */
  void exportGame(const Project &project, const Path &destination);
};

} // namespace quoll::editor
