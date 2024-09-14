#pragma once

#include "LogStream.h"
#include "LoggerCore.h"

namespace quoll {

class Logger {
public:
#if defined(QUOLL_DEBUG)
  static constexpr LogSeverity DefaultSeverity = LogSeverity::Debug;
#else
  static constexpr LogSeverity DefaultSeverity = LogSeverity::Info;
#endif

public:
  Logger(LogSeverity minSeverity = DefaultSeverity);

  void setMinSeverity(LogSeverity minSeverity);

  void setTransport(LogTransport transport);

  inline LogStream debug() { return log(LogSeverity::Debug); }

  inline LogStream info() { return log(LogSeverity::Info); }

  inline LogStream warning() { return log(LogSeverity::Warning); }

  inline LogStream error() { return log(LogSeverity::Error); }

  inline LogStream fatal() { return log(LogSeverity::Fatal); }

private:
  LogStream log(LogSeverity severity);

private:
  LogSeverity mMinSeverity;

  LogTransport mTransport;
};

} // namespace quoll
