#pragma once

#include "InputDataType.h"

namespace quoll::input {

bool exists(const String &key);

int get(const String &key);

InputDataType getKeyDataType(const String &key);

} // namespace quoll::input
