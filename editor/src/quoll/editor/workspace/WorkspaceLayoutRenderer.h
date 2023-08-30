#pragma once

namespace quoll::editor {

/**
 * @brief Workspace layout renderer
 */
class WorkspaceLayoutRenderer {
public:
  /**
   * @brief Begin rendering
   *
   * @retval true Rendering started successfully
   * @retval false Rendering failed
   */
  static bool begin();

  /**
   * @brief End rendering
   */
  static void end();

  /**
   * Reset up the workspace layout
   */
  static void reset();

  /**
   * @brief Resize layout
   */
  static void resize();
};

} // namespace quoll::editor
