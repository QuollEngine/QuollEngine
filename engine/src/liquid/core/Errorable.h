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
   * @param result_ Result object
   */
  Errorable(const TResult &result_) : result(result_) {}

  /**
   * @brief Create errorable with no error
   *
   * @param result_ Result object
   */
  Errorable(TResult &&result_) : result(result_) {}

  /**
   * @brief Create errorable with error
   *
   * @param error_ Error enum
   */
  Errorable(TError error_) : error(error_) {}

  /**
   * @brief Check if errorable has error
   *
   * @retval true Has error
   * @retval false Does not have error
   */
  inline bool hasError() const { return error != TError::None; }

  /**
   * @brief Check if errorable has result
   *
   * @retval true Has result
   * @retval false Does not have result
   */
  inline bool hasResult() const { return result.has_value(); }

  /**
   * @brief Get result
   *
   * @return Result
   */
  inline TResult getResult() const { return result.value(); }

  /**
   * @brief Get error
   *
   * @return Error
   */
  inline TError getError() const { return error; }

private:
  std::optional<TResult> result;
  TError error = TError::None;
};

} // namespace liquid
