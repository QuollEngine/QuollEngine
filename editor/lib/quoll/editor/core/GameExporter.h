#pragma once

#include "quoll/editor/project/Project.h"

namespace quoll::editor {

class GameExporter {
public:
  void exportGame(const Project &project, const Path &destination);
};

} // namespace quoll::editor
