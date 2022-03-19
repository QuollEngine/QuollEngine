#pragma once

namespace liquid {

/**
 * @brief Errorable type
 *
 * Determines if a result of a funciton
 * or an operation is error or success
 *
 * @tparam TResult Result object type
 * @tparam TError Error enum type
 */
template <class TResult, class TError> class Errorable {
public:
  /**
   * @brief Create errorable with no error
   *
   * @param result Result object
   */
  Errorable(const TResult &result) : mResult(result) {}

  /**
   * @brief Create errorable with no error
   *
   * @param result Result object
   */
  Errorable(TResult &&result) : mResult(result) {}

  /**
   * @brief Create errorable with error
   *
   * @param error Error enum
   */
  Errorable(TError error) : mError(error) {}

  /**
   * @brief Check if errorable has error
   *
   * @retval true Has error
   * @retval false Does not have error
   */
  inline bool hasError() const { return mError != TError::None; }

  /**
   * @brief Check if errorable has result
   *
   * @retval true Has result
   * @retval false Does not have result
   */
  inline bool hasResult() const { return mResult.has_value(); }

  /**
   * @brief Get result
   *
   * @return Result
   */
  inline TResult getResult() const { return mResult.value(); }

  /**
   * @brief Get error
   *
   * @return Error
   */
  inline TError getError() const { return mError; }

private:
  std::optional<TResult> mResult;
  TError mError = TError::None;
};

} // namespace liquid
