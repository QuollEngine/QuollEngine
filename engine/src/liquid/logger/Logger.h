#pragma once

#include "LoggerCore.h"
#include "LogStream.h"

namespace liquid {

/**
 * @brief Logger
 *
 * Logs data into different streams
 */
class Logger {
public:
#if defined(LIQUID_DEBUG)
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

  /**
   * @brief Set log transport
   *
   * @param transport Log transport
   */
  void setTransport(LogTransport transport);

private:
  LogSeverity mMinSeverity;

  LogTransport mTransport;

private:
  static constexpr auto NoopTransport = [](LogSeverity, LogTimestamp, String) {
  };
};

} // namespace liquid
