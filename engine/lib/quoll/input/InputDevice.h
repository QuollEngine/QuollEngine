#pragma once

namespace quoll {

enum class InputDeviceType { Keyboard, Mouse, Gamepad, Unknown };

using InputStateValue = std::variant<bool, f32, glm::vec2>;

struct InputDevice {
  InputDeviceType type = InputDeviceType::Unknown;

  String name;

  u32 index = 0;

  std::function<InputStateValue(int key)> stateFn;
};

} // namespace quoll
