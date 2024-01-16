#pragma once

namespace quoll {

template <class TData> class Result {
  enum class OkEnum {};
  enum class ErrorEnum {};

public:
  static Result<TData> Ok(const TData &data,
                          const std::vector<String> &warnings = {}) {
    return Result<TData>(OkEnum{}, data, warnings);
  }

  static Result<TData> Error(StringView error) {
    return Result<TData>(ErrorEnum{}, error);
  }

public:
  Result(OkEnum _, const TData &data, const std::vector<String> &warnings)
      : mData(data), mWarnings(warnings) {}

  Result(ErrorEnum _, StringView error) : mError(error) {}

  inline bool hasData() const { return mData.has_value(); }

  const TData &getData() const { return mData.value(); }

  TData &getData() { return mData.value(); }

  inline bool hasWarnings() const { return !mWarnings.empty(); }

  inline const std::vector<String> &getWarnings() const { return mWarnings; }

  inline bool hasError() const { return !mError.empty(); }

  inline const String &getError() const { return mError; }

private:
  std::optional<TData> mData;
  std::vector<String> mWarnings;
  String mError;
};

} // namespace quoll
