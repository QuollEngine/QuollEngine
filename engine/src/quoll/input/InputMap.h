#pragma once

#include "quoll/asset/AssetHandle.h"
#include "InputDataType.h"
#include "InputMapAsset.h"

namespace quoll {

struct InputMapAssetRef {
  AssetHandle<InputMapAsset> handle;

  usize defaultScheme = 0;
};

struct InputMapBindingTable {
  std::unordered_map<int, usize> inputKeyToCommandMap;

  /**
   * Used to identify what part of a value
   * the key represents
   *
   * Used for non-boolean value types
   */
  std::unordered_map<int, InputDataTypeField> inputKeyFields;
};

struct InputMap {
  std::vector<InputDataType> commandDataTypes;

  /**
   * Command name to internal command key map
   */
  std::unordered_map<String, usize> commandNameMap;

  std::vector<std::variant<bool, glm::vec2>> commandValues;

  /**
   * Binding tables based on scheme
   */
  std::vector<InputMapBindingTable> schemes;

  /**
   * Scheme name to internal scheme key map
   */
  std::unordered_map<String, usize> schemeNameMap;

  usize activeScheme = 0;
};

} // namespace quoll
