#pragma once

namespace liquid {

template <class TData> class Result {
  enum class OkEnum {};
  enum class ErrorEnum {};

public:
  static Result<TData> Ok(const TData &data,
                          const std::vector<String> &warnings = {}) {
    return Result<TData>(OkEnum{}, data, warnings);
  }

  static Result<TData> Error(const String &error) {
    return Result<TData>(ErrorEnum{}, error);
  }

public:
  Result(OkEnum, const TData &data, const std::vector<String> &warnings)
      : mData(data), mWarnings(warnings) {}
  Result(ErrorEnum, const String &error) : mError(error) {}

  const TData &getData() const { return mData.value(); }

  inline bool hasWarnings() const { return !mWarnings.empty(); }

  inline bool hasError() const { return !mError.empty(); }

  inline const String &getError() const { return mError; }

  inline bool hasData() const { return mData.has_value(); }

private:
  std::optional<TData> mData;
  std::vector<String> mWarnings;
  String mError;
};

} // namespace liquid
