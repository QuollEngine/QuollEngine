#pragma once

#include "liquid/logger/Logger.h"

namespace quoll::editor {

/**
 * @brief In memory storage for logs
 */
class LogMemoryStorage {
  struct LogEntry {
    LogSeverity severity;
    LogTimestamp timestamp;
    String message;
  };

public:
  /**
   * @brief Create log transport
   *
   * @return Log transport
   */
  LogTransport createTransport();

  /**
   * @brief Get all log entries
   *
   * @return Log entries
   */
  inline const std::vector<LogEntry> &getEntries() const { return mEntries; }

  /**
   * @brief Clear stored logs
   */
  void clear();

private:
  std::vector<LogEntry> mEntries;
};

} // namespace quoll::editor
