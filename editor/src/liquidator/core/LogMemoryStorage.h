#pragma once

#include "liquid/logger/Logger.h"

namespace liquidator {

/**
 * @brief In memory storage for logs
 */
class LogMemoryStorage {
  struct LogEntry {
    liquid::LogSeverity severity;
    liquid::LogTimestamp timestamp;
    liquid::String message;
  };

public:
  /**
   * @brief Create log transport
   *
   * @return Log transport
   */
  liquid::LogTransport createTransport();

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

} // namespace liquidator
