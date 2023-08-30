#include "quoll/core/Base.h"
#include "LogMemoryStorage.h"

namespace quoll::editor {

LogTransport LogMemoryStorage::createTransport() {
  return [this](LogSeverity severity, LogTimestamp timestamp, String message) {
    mEntries.push_back({severity, timestamp, message});
  };
}

void LogMemoryStorage::clear() { mEntries.clear(); }

} // namespace quoll::editor
