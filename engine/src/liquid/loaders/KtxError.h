#pragma once

#include <ktx.h>
#include "liquid/core/Base.h"

namespace liquid {

/**
 * @brief KTX Error
 *
 * This exception should be used for all
 * KTX related fatal errors
 */
class KtxError : public std::runtime_error {
public:
  /**
   * @brief Error constructor
   *
   * @param what Error message
   * @param resultCode Ktx result code
   */
  KtxError(const String &what, ktx_error_code_e resultCode);
};

} // namespace liquid
