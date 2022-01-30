#pragma once

namespace liquid {

/**
 * @brief Null output stream buffer
 *
 * Used as buffer of Null output Stream
 */
class NullOutStreamBuf : public std::streambuf {
public:
  /**
   * @brief Does nothing
   *
   * @param s character array
   * @param n stream size
   * @return Stream size
   */
  std::streamsize xsputn(const char *s, std::streamsize n) override;

  /**
   * @brief Does nothing
   *
   * @param c Character
   * @return Returns character with integer value 1
   */
  int overflow(int c) override;
};

/**
 * @brief Null output stream
 *
 * Used to putput nothing
 */
class NullOutStream : public std::ostream {
public:
  NullOutStream();

private:
  NullOutStreamBuf buf;
};

/**
 * @brief Output Stream enhancer for logging
 *
 * Adds new line at the end of stream
 * as soon as this object is destructed
 *
 * This class is used by Logger to
 * automatically add a new line.
 */
class LoggerStream {
public:
  /**
   * @brief Stores reference to output stream
   *
   * @param stream Output Stream
   */
  LoggerStream(std::ostream &stream);
  LoggerStream(LoggerStream &&rhs);

  /**
   * @brief Destructor
   *
   * This destructor adds endl to the stream
   */
  ~LoggerStream();

  LoggerStream(const LoggerStream &) = delete;
  LoggerStream &operator=(const LoggerStream &) = delete;
  LoggerStream &operator=(LoggerStream &&) = delete;

  /**
   * @brief Output stream operator overload

   * @tparam T Value to write to stream
   * @param value Output Stream
   * @return Rvalue reference of itself
   */
  template <typename T> LoggerStream &&operator<<(T &&val) {
    stream << std::forward<T>(val);
    return std::move(*this);
  }

private:
  std::ostream &stream;
  bool alive = true;
};

/**
 * @brief Logger
 *
 * Logs data into different streams
 */
class Logger {
public:
  enum Severity { Debug = 0, Info = 1, Warning = 2, Error = 3, Fatal = 4 };

  /**
   * @brief Gets human readable severity string
   *
   * Converts Severity enumeration to string
   *
   * @param severity Log Severity
   * @return Severity in human readable
   */
  static String getSeverityString(Severity severity);

public:
  /**
   * @brief Sets minimum severity to log from
   *
   * @param minSeverity Minimum severity
   */
  Logger(Severity minSeverity = Warning);

  /**
   * @brief Formats timestamp and severity
   *
   * @param severity Log severity
   * @return timestamp Log timestamp
   */
  String format(Severity severity,
                std::chrono::time_point<std::chrono::system_clock> &&timestamp);

  /**
   * @brief Creates logger stream with metadata attached
   *
   * If severity is lower than minimum severity
   * the function will create a logger stream
   * with null output stream
   *
   * @param severity Log severity
   * @return LoggerStream with output stream
   */
  LoggerStream log(Severity severity);

private:
  Severity minSeverity;
  NullOutStream nullOut;
};

} // namespace liquid
