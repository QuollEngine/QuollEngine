#pragma once

namespace quoll {

class Error {
public:
  constexpr Error() = default;
  constexpr Error(const String &message) : mMessage(message) {}

  inline const String &message() const { return mMessage; }

  operator String() const { return mMessage; }

private:
  String mMessage;
};

template <class TData> class Result {
public:
  constexpr Result() : mError("No data"){};

  constexpr Result(const TData &data) : mData(data) {}
  constexpr Result(const TData &data, const std::vector<String> &warnings)
      : mData(data), mWarnings(warnings) {}

  constexpr Result(const Error &error) : mError(error) {}

  /**
   * Check if result is data or error
   *
   * @retval true Result is data
   * @retval false Result is error
   */
  inline operator bool() const { return mData.has_value(); }

  operator TData() { return mData.value(); }

  operator const TData() const { return mData.value(); }

  inline bool hasWarnings() const { return !mWarnings.empty(); }

  inline const TData &data() const { return mData.value(); }
  inline TData &data() { return mData.value(); }
  inline const Error &error() const { return mError; }
  inline const std::vector<String> &warnings() const { return mWarnings; }

private:
  std::optional<TData> mData;
  std::vector<String> mWarnings;
  Error mError;
};

template <> class Result<void> {
public:
  constexpr Result() : mHasValue(true) {}

  constexpr Result(const std::vector<String> &warnings)
      : mHasValue(true), mWarnings(warnings) {}

  constexpr Result(const Error &error) : mError(error) {}

  /**
   * Check if result is data or error
   *
   * @retval true Result is data
   * @retval false Result is error
   */
  inline operator bool() const { return mHasValue; }

  inline const Error &error() const { return mError; }
  inline const std::vector<String> &warnings() const { return mWarnings; }
  inline bool hasWarnings() const { return !mWarnings.empty(); }

private:
  Error mError;
  std::vector<String> mWarnings;
  bool mHasValue = false;
};

static constexpr Result<void> Ok(const std::vector<String> &warnings = {}) {
  return Result<void>(warnings);
}

} // namespace quoll
