#include "liquid/core/Base.h"
#include "LogMemoryStorage.h"

namespace liquidator {

liquid::LogTransport LogMemoryStorage::createTransport() {
  return [this](liquid::LogSeverity severity, liquid::LogTimestamp timestamp,
                liquid::String message) {
    mEntries.push_back({severity, timestamp, message});
  };
}

void LogMemoryStorage::clear() { mEntries.clear(); }

} // namespace liquidator
