#pragma once

namespace quoll {

/**
 * @brief Input map command types
 */
enum InputMapCommandType { Boolean, Axis2d };

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
 * @brief Input map keyboard binding
 */
struct InputMapKeyboard {
  /**
   * Keyboard key
   */
  int key;
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
  InputMapCommandType type;
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
   * Binding values
   */
  std::vector<InputMapKeyboard> binding;
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
