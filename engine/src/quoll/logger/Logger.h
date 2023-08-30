#pragma once

#include "LoggerCore.h"
#include "LogStream.h"

namespace quoll {

/**
 * @brief Logger
 *
 * Logs data into different streams
 */
class Logger {
public:
#if defined(QUOLL_DEBUG)
  /**
   * @brief Default severity
   *
   * Default severity is debug in debug mode
   */
  static constexpr LogSeverity DefaultSeverity = LogSeverity::Debug;

#else
  /**
   * @brief Default severity
   *
   * Default severity is warning in release mode
   */
  static constexpr LogSeverity DefaultSeverity = LogSeverity::Warning;
#endif

public:
  /**
   * @brief Set minimum severity to log from
   *
   * @param minSeverity Minimum severity
   */
  Logger(LogSeverity minSeverity = DefaultSeverity);

  /**
   * @brief Set log transport
   *
   * @param transport Log transport
   */
  void setTransport(LogTransport transport);

  /**
   * @brief Create log stream for debug severity
   *
   * @return Log stream for debug
   */
  inline LogStream debug() { return log(LogSeverity::Debug); }

  /**
   * @brief Create log stream for info severity
   *
   * @return Log stream for info
   */
  inline LogStream info() { return log(LogSeverity::Info); }

  /**
   * @brief Create log stream for warning severity
   *
   * @return Log stream for warning
   */
  inline LogStream warning() { return log(LogSeverity::Warning); }

  /**
   * @brief Create log stream for error severity
   *
   * @return Log stream for error
   */
  inline LogStream error() { return log(LogSeverity::Error); }

  /**
   * @brief Create log stream for fatal severity
   *
   * @return Log stream for fatal
   */
  inline LogStream fatal() { return log(LogSeverity::Fatal); }

private:
  /**
   * @brief Create log stream
   *
   * If severity is lower than minimum severity
   * the function will create a logger stream
   * with noop transport
   *
   * @param severity Log severity
   * @return Log stream
   */
  LogStream log(LogSeverity severity);

private:
  LogSeverity mMinSeverity;

  LogTransport mTransport;
};

} // namespace quoll
