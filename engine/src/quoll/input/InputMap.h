#pragma once

#include "quoll/asset/Asset.h"
#include "InputDataType.h"

namespace quoll {

/**
 * @brief Input map asset reference
 *
 * Stores reference to the asset
 */
struct InputMapAssetRef {
  /**
   * Asset handle
   */
  InputMapAssetHandle handle = InputMapAssetHandle::Null;
};

/**
 * @brief Input map component
 */
struct InputMap {
  /**
   * Data types representing the commands
   */
  std::vector<InputDataType> commandDataTypes;

  /**
   * Command name to internal command key map
   */
  std::unordered_map<String, size_t> commandNameMap;

  /**
   * Input key to command map
   */
  std::unordered_map<int, size_t> inputKeyToCommandMap;

  /**
   * Command values
   */
  std::vector<std::variant<bool, glm::vec2>> commandValues;

  /**
   * Used to identify what part of a value
   * the key represents
   *
   * Used non-boolean value types
   */
  std::unordered_map<int, InputDataTypeField> inputKeyFields;
};

} // namespace quoll
