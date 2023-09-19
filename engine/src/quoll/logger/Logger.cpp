#include "quoll/core/Base.h"

#include "NoopLogTransport.h"
#include "Logger.h"

namespace quoll {

Logger::Logger(LogSeverity minSeverity) : mMinSeverity(minSeverity) {}

void Logger::setMinSeverity(LogSeverity minSeverity) {
  mMinSeverity = minSeverity;
}

LogStream Logger::log(LogSeverity severity) {
  auto timestamp = std::chrono::system_clock::now();

  return LogStream(severity, timestamp,
                   severity >= mMinSeverity ? mTransport : NoopLogTransport);
}

void Logger::setTransport(LogTransport transport) { mTransport = transport; }

} // namespace quoll
