#pragma once

#include <ktx.h>

namespace quoll {

class KtxError : public std::runtime_error {
public:
  KtxError(const String &what, ktx_error_code_e resultCode);
};

} // namespace quoll
