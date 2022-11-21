#pragma once

#include "liquidator/core/LogMemoryStorage.h"

namespace liquidator {

/**
 * @brief Log viewer
 */
class LogViewer {
public:
  /**
   * @brief Render log viewer
   *
   * @param engineLogs Engine logs
   */
  void render(LogMemoryStorage &engineLogs);

private:
  size_t mEngineLogsSize = 0;
};

} // namespace liquidator
