#pragma once

#include "quoll/input/InputDataType.h"

namespace quoll {

/**
 * @brief Input map scheme
 */
struct InputMapScheme {
  /**
   * Scheme name
   */
  String name;
};

/**
 * @brief Input map command
 */
struct InputMapCommand {
  /**
   * Command name
   */
  String name;

  /**
   * Command type
   */
  InputDataType type;
};

using InputMapValue = int;

using InputMapAxisSegment = std::array<InputMapValue, 2>;

using InputMapAxis1dValue = std::variant<InputMapValue, InputMapAxisSegment>;

/**
 * @brief 2d axis value
 */
struct InputMapAxis2dValue {
  /**
   * 1d axis value for x
   */
  InputMapAxis1dValue x;

  /**
   * 1d axis value for y
   */
  InputMapAxis1dValue y;
};

/**
 * @brief Input map binding
 */
struct InputMapBinding {
  /**
   * Scheme index
   */
  size_t scheme = 0;

  /**
   * Command index
   */
  size_t command = 0;

  /**
   * Binding value
   */
  std::variant<InputMapValue, InputMapAxis2dValue> value;
};

/**
 * @brief Input map asset
 */
struct InputMapAsset {
  /**
   * Input map schemes
   */
  std::vector<InputMapScheme> schemes;

  /**
   * Input map commands
   */
  std::vector<InputMapCommand> commands;

  /**
   * Input map bindings
   */
  std::vector<InputMapBinding> bindings;
};

} // namespace quoll
