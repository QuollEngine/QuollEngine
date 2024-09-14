#pragma once

#include "quoll/logger/Logger.h"

namespace quoll::editor {

class LogMemoryStorage {
  struct LogEntry {
    LogSeverity severity;
    LogTimestamp timestamp;
    String message;
  };

public:
  LogTransport createTransport();

  inline const std::vector<LogEntry> &getEntries() const { return mEntries; }

  void clear();

private:
  std::vector<LogEntry> mEntries;
};

} // namespace quoll::editor
