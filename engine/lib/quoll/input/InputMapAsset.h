#pragma once

#include "quoll/input/InputDataType.h"

namespace quoll {

struct InputMapScheme {
  String name;
};

struct InputMapCommand {
  String name;

  InputDataType type;
};

using InputMapValue = int;

using InputMapAxisSegment = std::array<InputMapValue, 2>;

using InputMapAxis1dValue = std::variant<InputMapValue, InputMapAxisSegment>;

struct InputMapAxis2dValue {
  InputMapAxis1dValue x;

  InputMapAxis1dValue y;
};

struct InputMapBinding {
  usize scheme = 0;

  usize command = 0;

  std::variant<InputMapValue, InputMapAxis2dValue> value;
};

struct InputMapAsset {
  std::vector<InputMapScheme> schemes;

  std::vector<InputMapCommand> commands;

  std::vector<InputMapBinding> bindings;
};

} // namespace quoll
