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
  explicit LogStream(LogSeverity severity, LogTimestamp timestamp,
                     LogTransport transport);

  explicit LogStream(LogStream &&rhs) noexcept;

  ~LogStream();

  LogStream(const LogStream &) = delete;
  LogStream &operator=(const LogStream &) = delete;
  LogStream &operator=(LogStream &&) = delete;

  template <class T> LogStream &operator<<(T &value) {
    mStream << value;
    return *this;
  }

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
