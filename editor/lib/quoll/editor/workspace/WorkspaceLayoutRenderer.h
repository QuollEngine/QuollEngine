#pragma once

namespace quoll::editor {

class WorkspaceLayoutRenderer {
public:
  static bool begin();

  static void end();

  static void reset();

  static void resize();
};

} // namespace quoll::editor
