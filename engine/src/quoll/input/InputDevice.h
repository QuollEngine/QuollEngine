#pragma once

namespace quoll {

enum class InputDeviceType { Keyboard, Mouse, Gamepad, Unknown };

using InputStateValue = std::variant<bool, float, glm::vec2>;

/**
 * @brief Input device
 */
struct InputDevice {
  /**
   * Device type
   */
  InputDeviceType type = InputDeviceType::Unknown;

  /**
   * Device name
   */
  String name;

  /**
   * Device index
   */
  uint32_t index = 0;

  /**
   * Device state function
   */
  std::function<InputStateValue(int key)> stateFn;
};

} // namespace quoll
