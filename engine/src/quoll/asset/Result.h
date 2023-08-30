#pragma once

namespace quoll {

/**
 * @brief Result object
 *
 * Used as a replacement for exceptions
 *
 * @tparam TData Data type
 */
template <class TData> class Result {
  enum class OkEnum {};
  enum class ErrorEnum {};

public:
  /**
   * @brief Create Ok result
   *
   * @param data Data
   * @param warnings Warnings
   * @return Ok result
   */
  static Result<TData> Ok(const TData &data,
                          const std::vector<String> &warnings = {}) {
    return Result<TData>(OkEnum{}, data, warnings);
  }

  /**
   * @brief Create error result
   *
   * @param error Error
   * @return Error result
   */
  static Result<TData> Error(StringView error) {
    return Result<TData>(ErrorEnum{}, error);
  }

public:
  /**
   * @brief Create Ok result
   *
   * @param _ Ok enum
   * @param data Data
   * @param warnings Warnings
   */
  Result(OkEnum _, const TData &data, const std::vector<String> &warnings)
      : mData(data), mWarnings(warnings) {}

  /**
   * @brief Create Error result
   *
   * @param _ Error enum
   * @param error Error
   */
  Result(ErrorEnum _, StringView error) : mError(error) {}

  /**
   * @brief Check if result has data
   *
   * @retval true Result has data
   * @retval false Result does n not have data
   */
  inline bool hasData() const { return mData.has_value(); }

  /**
   * @brief Get data
   *
   * @return Data
   */
  const TData &getData() const { return mData.value(); }

  /**
   * @brief Get data
   *
   * @return Data
   */
  TData &getData() { return mData.value(); }

  /**
   * @brief Check if result has warnings
   *
   * @retval true Result has warnings
   * @retval false Result does not have warnings
   */
  inline bool hasWarnings() const { return !mWarnings.empty(); }

  /**
   * @brief Get warnings
   *
   * @return Warnings
   */
  inline const std::vector<String> &getWarnings() const { return mWarnings; }

  /**
   * @brief Check if result has error
   *
   * @retval true Result has error
   * @retval false Result does not have error
   */
  inline bool hasError() const { return !mError.empty(); }

  /**
   * @brief Get error
   *
   * @return Error
   */
  inline const String &getError() const { return mError; }

private:
  std::optional<TData> mData;
  std::vector<String> mWarnings;
  String mError;
};

} // namespace quoll
