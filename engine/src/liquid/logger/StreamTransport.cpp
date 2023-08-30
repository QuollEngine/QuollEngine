#include "liquid/core/Base.h"
#include "StreamTransport.h"

namespace quoll {

/**
 * @brief Format log headers
 *
 * @param severity Severity
 * @param timestamp Timestamp
 * @return Formatted header string
 */
String formatLogHeaders(
    LogSeverity severity,
    std::chrono::time_point<std::chrono::system_clock> &&timestamp) {

  auto time = std::chrono::system_clock::to_time_t(timestamp);

  std::stringstream str;
  str << std::put_time(std::localtime(&time), "%X") << " ["
      << getLogSeverityString(severity) << "] ";
  return str.str();
}

LogTransport createStreamTransport(std::ostream &stream) {
  return
      [&stream](LogSeverity severity, LogTimestamp timestamp, String message) {
        stream << formatLogHeaders(severity, std::move(timestamp)) << message
               << std::endl;
      };
}

} // namespace quoll
