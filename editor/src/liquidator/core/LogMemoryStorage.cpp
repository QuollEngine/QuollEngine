#include "liquid/core/Base.h"
#include "LogMemoryStorage.h"

namespace liquid::editor {

LogTransport LogMemoryStorage::createTransport() {
  return [this](LogSeverity severity, LogTimestamp timestamp, String message) {
    mEntries.push_back({severity, timestamp, message});
  };
}

void LogMemoryStorage::clear() { mEntries.clear(); }

} // namespace liquid::editor
