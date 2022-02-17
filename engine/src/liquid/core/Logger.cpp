#include "liquid/core/Base.h"
#include "Logger.h"

#pragma warning(disable : 4996)

namespace liquid {

std::streamsize NullOutStreamBuf::xsputn(const char *s, std::streamsize n) {
  return n;
}

int NullOutStreamBuf::overflow(int c) { return 1; }

NullOutStream::NullOutStream() : std::ostream(&buf) {}

LoggerStream::LoggerStream(std::ostream &stream_) : stream(stream_){};
LoggerStream::LoggerStream(LoggerStream &&rhs) : stream(rhs.stream) {
  rhs.alive = false;
}
LoggerStream::~LoggerStream() {
  if (alive) {
    stream << std::endl;
  }
}

std::string Logger::getSeverityString(Severity severity) {
  switch (severity) {
  case Severity::Debug:
    return "DEBUG";
  case Severity::Info:
    return "INFO";
  case Severity::Warning:
    return "WARNING";
  case Severity::Error:
    return "ERROR";
  case Severity::Fatal:
    return "FATAL";
  default:
    return "NONE";
  }
}

Logger::Logger(Severity minSeverity_) : minSeverity(minSeverity_) {}

String
Logger::format(Severity severity,
               std::chrono::time_point<std::chrono::system_clock> &&timestamp) {

  auto time = std::chrono::system_clock::to_time_t(timestamp);

  std::stringstream str;
  str << std::put_time(std::localtime(&time), "%Y-%m-%d %X") << " ["
      << Logger::getSeverityString(severity) << "] ";
  return str.str();
}

LoggerStream Logger::log(Severity severity) {
  auto timestamp = std::chrono::system_clock::now();
  auto &&stream = LoggerStream(severity >= minSeverity ? std::cout : nullOut);
  return std::move(stream << format(severity, std::move(timestamp)));
}

} // namespace liquid
