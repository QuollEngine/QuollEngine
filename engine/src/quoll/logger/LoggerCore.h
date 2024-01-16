#pragma once

namespace quoll {

enum class LogSeverity {
  Debug = 0,
  Info = 1,
  Warning = 2,
  Error = 3,
  Fatal = 4
};

using LogTimestamp = std::chrono::time_point<std::chrono::system_clock>;

using LogTransport = std::function<void(LogSeverity, LogTimestamp, String)>;

static String getLogSeverityString(LogSeverity severity) {
  switch (severity) {
  case LogSeverity::Debug:
    return "Debug";
  case LogSeverity::Info:
    return "Info";
  case LogSeverity::Warning:
    return "Warning";
  case LogSeverity::Error:
    return "Error";
  case LogSeverity::Fatal:
    return "Fatal";
  default:
    return "None";
  }
}

} // namespace quoll
