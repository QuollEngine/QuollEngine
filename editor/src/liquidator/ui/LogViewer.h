#pragma once

#include "liquidator/core/LogMemoryStorage.h"

namespace liquid::editor {

/**
 * @brief Log viewer
 */
class LogViewer {
public:
  /**
   * @brief Render log viewer
   *
   * @param systemLogs System logs
   * @param userLogs User logs
   */
  void render(LogMemoryStorage &systemLogs, LogMemoryStorage &userLogs);

private:
  /**
   * @brief Render log container
   *
   * @param name Container name
   * @param logStorage Log storage
   * @param logSize Current log size
   * @param width Container width
   */
  void renderLogContainer(const String &name, LogMemoryStorage &logStorage,
                          size_t &logSize, float width);

private:
  size_t mSystemLogSize = 0;
  size_t mUserLogSize = 0;
};

} // namespace liquid::editor
