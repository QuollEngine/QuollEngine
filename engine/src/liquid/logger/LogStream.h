#pragma once

#include "LoggerCore.h"

namespace quoll {

/**
 * @brief Log stream
 *
 * Output messages to the stream
 * and flush them on destruct
 */
class LogStream {
public:
  /**
   * @brief Create log stream
   *
   * @param severity Log severity
   * @param timestamp Log timestamp
   * @param transport Log transport
   */
  explicit LogStream(LogSeverity severity, LogTimestamp timestamp,
                     LogTransport transport);

  /**
   * @brief Move constructor
   *
   * @param rhs Other log stream
   */
  explicit LogStream(LogStream &&rhs);

  /**
   * @brief Flush log stream
   *
   * Flushes the stream using
   * transport
   */
  ~LogStream();

  LogStream(const LogStream &) = delete;
  LogStream &operator=(const LogStream &) = delete;
  LogStream &operator=(LogStream &&) = delete;

  /**
   * @brief Output stream operator
   *
   * @tparam T Value type
   * @param value Value
   * @return This log stream
   */
  template <class T> LogStream &operator<<(T &value) {
    mStream << value;
    return *this;
  }

  /**
   * @brief Output stream operator
   *
   * @tparam T Value type
   * @param value Value
   * @return This log stream
   */
  template <class T> LogStream &operator<<(const T &value) {
    mStream << value;
    return *this;
  }

private:
  std::stringstream mStream;
  LogSeverity mSeverity;
  LogTimestamp mTimestamp;
  LogTransport mTransport;
};

} // namespace quoll
