#include "liquid/core/Base.h"
#include "Logger.h"

namespace liquid {

Logger::Logger(LogSeverity minSeverity) : mMinSeverity(minSeverity) {}

LogStream Logger::log(LogSeverity severity) {
  auto timestamp = std::chrono::system_clock::now();

  return LogStream(severity, timestamp,
                   severity >= mMinSeverity ? mTransport : NoopTransport);
}

void Logger::setTransport(LogTransport transport) { mTransport = transport; }

} // namespace liquid
