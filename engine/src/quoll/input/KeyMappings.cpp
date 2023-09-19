#include "quoll/core/Base.h"
#include "KeyMappings.h"

#include <GLFW/glfw3.h>

namespace quoll::input {

static constexpr int KeyboardStart = 0;
static constexpr int MouseStart = 1000;
static constexpr int GamepadStart = 2000;
static constexpr int GamepadAxisStart = 2100;

static constexpr int MouseMove = 10;

int getKeyboardKeyFromGlfw(int glfw) { return KeyboardStart + glfw; }

int getMouseButtonFromGlfw(int glfw) { return MouseStart + glfw; }

int getMouseMove() { return MouseMove; }

int getGamepadButtonFromGlfw(int glfw) { return GamepadStart + glfw; }

int getGamepadAxisFromGlfw(int glfw) { return GamepadAxisStart + glfw; }

static const std::unordered_map<String, int> Mapping{
    // letters
    {"KEY_A", getKeyboardKeyFromGlfw(GLFW_KEY_A)},
    {"KEY_B", getKeyboardKeyFromGlfw(GLFW_KEY_B)},
    {"KEY_C", getKeyboardKeyFromGlfw(GLFW_KEY_C)},
    {"KEY_D", getKeyboardKeyFromGlfw(GLFW_KEY_D)},
    {"KEY_E", getKeyboardKeyFromGlfw(GLFW_KEY_E)},
    {"KEY_F", getKeyboardKeyFromGlfw(GLFW_KEY_F)},
    {"KEY_G", getKeyboardKeyFromGlfw(GLFW_KEY_G)},
    {"KEY_H", getKeyboardKeyFromGlfw(GLFW_KEY_H)},
    {"KEY_I", getKeyboardKeyFromGlfw(GLFW_KEY_I)},
    {"KEY_J", getKeyboardKeyFromGlfw(GLFW_KEY_J)},
    {"KEY_K", getKeyboardKeyFromGlfw(GLFW_KEY_K)},
    {"KEY_L", getKeyboardKeyFromGlfw(GLFW_KEY_L)},
    {"KEY_M", getKeyboardKeyFromGlfw(GLFW_KEY_M)},
    {"KEY_N", getKeyboardKeyFromGlfw(GLFW_KEY_N)},
    {"KEY_O", getKeyboardKeyFromGlfw(GLFW_KEY_O)},
    {"KEY_P", getKeyboardKeyFromGlfw(GLFW_KEY_P)},
    {"KEY_Q", getKeyboardKeyFromGlfw(GLFW_KEY_Q)},
    {"KEY_R", getKeyboardKeyFromGlfw(GLFW_KEY_R)},
    {"KEY_S", getKeyboardKeyFromGlfw(GLFW_KEY_S)},
    {"KEY_T", getKeyboardKeyFromGlfw(GLFW_KEY_T)},
    {"KEY_U", getKeyboardKeyFromGlfw(GLFW_KEY_U)},
    {"KEY_V", getKeyboardKeyFromGlfw(GLFW_KEY_V)},
    {"KEY_W", getKeyboardKeyFromGlfw(GLFW_KEY_W)},
    {"KEY_X", getKeyboardKeyFromGlfw(GLFW_KEY_X)},
    {"KEY_Y", getKeyboardKeyFromGlfw(GLFW_KEY_Y)},
    {"KEY_Z", getKeyboardKeyFromGlfw(GLFW_KEY_Z)},

    // digits
    {"KEY_0", getKeyboardKeyFromGlfw(GLFW_KEY_0)},
    {"KEY_1", getKeyboardKeyFromGlfw(GLFW_KEY_1)},
    {"KEY_2", getKeyboardKeyFromGlfw(GLFW_KEY_2)},
    {"KEY_3", getKeyboardKeyFromGlfw(GLFW_KEY_3)},
    {"KEY_4", getKeyboardKeyFromGlfw(GLFW_KEY_4)},
    {"KEY_5", getKeyboardKeyFromGlfw(GLFW_KEY_5)},
    {"KEY_6", getKeyboardKeyFromGlfw(GLFW_KEY_6)},
    {"KEY_7", getKeyboardKeyFromGlfw(GLFW_KEY_7)},
    {"KEY_8", getKeyboardKeyFromGlfw(GLFW_KEY_8)},
    {"KEY_9", getKeyboardKeyFromGlfw(GLFW_KEY_9)},

    // special
    {"KEY_COMMA", getKeyboardKeyFromGlfw(GLFW_KEY_COMMA)},
    {"KEY_MINUS", getKeyboardKeyFromGlfw(GLFW_KEY_MINUS)},
    {"KEY_PERIOD", getKeyboardKeyFromGlfw(GLFW_KEY_PERIOD)},
    {"KEY_SLASH", getKeyboardKeyFromGlfw(GLFW_KEY_SLASH)},
    {"KEY_SEMICOLON", getKeyboardKeyFromGlfw(GLFW_KEY_SEMICOLON)},
    {"KEY_EQUAL", getKeyboardKeyFromGlfw(GLFW_KEY_EQUAL)},
    {"KEY_LEFT_BRACKET", getKeyboardKeyFromGlfw(GLFW_KEY_LEFT_BRACKET)},
    {"KEY_BACKSLASH", getKeyboardKeyFromGlfw(GLFW_KEY_BACKSLASH)},
    {"KEY_RIGHT_BRACKET", getKeyboardKeyFromGlfw(GLFW_KEY_RIGHT_BRACKET)},
    {"KEY_GRAVE_ACCENT", getKeyboardKeyFromGlfw(GLFW_KEY_GRAVE_ACCENT)},

    // command keys
    {"KEY_SPACE", getKeyboardKeyFromGlfw(GLFW_KEY_SPACE)},
    {"KEY_ESCAPE", getKeyboardKeyFromGlfw(GLFW_KEY_ESCAPE)},
    {"KEY_ENTER", getKeyboardKeyFromGlfw(GLFW_KEY_ENTER)},
    {"KEY_TAB", getKeyboardKeyFromGlfw(GLFW_KEY_TAB)},
    {"KEY_BACKSPACE", getKeyboardKeyFromGlfw(GLFW_KEY_BACKSPACE)},
    {"KEY_INSERT", getKeyboardKeyFromGlfw(GLFW_KEY_INSERT)},
    {"KEY_DELETE", getKeyboardKeyFromGlfw(GLFW_KEY_DELETE)},
    {"KEY_RIGHT", getKeyboardKeyFromGlfw(GLFW_KEY_RIGHT)},
    {"KEY_LEFT", getKeyboardKeyFromGlfw(GLFW_KEY_LEFT)},
    {"KEY_DOWN", getKeyboardKeyFromGlfw(GLFW_KEY_DOWN)},
    {"KEY_UP", getKeyboardKeyFromGlfw(GLFW_KEY_UP)},
    {"KEY_PAGE_UP", getKeyboardKeyFromGlfw(GLFW_KEY_PAGE_UP)},
    {"KEY_PAGE_DOWN", getKeyboardKeyFromGlfw(GLFW_KEY_PAGE_DOWN)},
    {"KEY_HOME", getKeyboardKeyFromGlfw(GLFW_KEY_HOME)},
    {"KEY_END", getKeyboardKeyFromGlfw(GLFW_KEY_END)},
    {"KEY_CAPS_LOCK", getKeyboardKeyFromGlfw(GLFW_KEY_CAPS_LOCK)},
    {"KEY_SCROLL_LOCK", getKeyboardKeyFromGlfw(GLFW_KEY_SCROLL_LOCK)},
    {"KEY_NUM_LOCK", getKeyboardKeyFromGlfw(GLFW_KEY_NUM_LOCK)},
    {"KEY_PRINT_SCREEN", getKeyboardKeyFromGlfw(GLFW_KEY_PRINT_SCREEN)},
    {"KEY_PAUSE", getKeyboardKeyFromGlfw(GLFW_KEY_PAUSE)},
    {"KEY_LEFT_SHIFT", getKeyboardKeyFromGlfw(GLFW_KEY_LEFT_SHIFT)},
    {"KEY_LEFT_CONTROL", getKeyboardKeyFromGlfw(GLFW_KEY_LEFT_CONTROL)},
    {"KEY_LEFT_ALT", getKeyboardKeyFromGlfw(GLFW_KEY_LEFT_ALT)},
    {"KEY_LEFT_SUPER", getKeyboardKeyFromGlfw(GLFW_KEY_LEFT_SUPER)},
    {"KEY_RIGHT_SHIFT", getKeyboardKeyFromGlfw(GLFW_KEY_RIGHT_SHIFT)},
    {"KEY_RIGHT_CONTROL", getKeyboardKeyFromGlfw(GLFW_KEY_RIGHT_CONTROL)},
    {"KEY_RIGHT_ALT", getKeyboardKeyFromGlfw(GLFW_KEY_RIGHT_ALT)},
    {"KEY_RIGHT_SUPER", getKeyboardKeyFromGlfw(GLFW_KEY_RIGHT_SUPER)},

    // Mouse
    {"MOUSE_LEFT", getMouseButtonFromGlfw(GLFW_MOUSE_BUTTON_LEFT)},
    {"MOUSE_RIGHT", getMouseButtonFromGlfw(GLFW_MOUSE_BUTTON_RIGHT)},
    {"MOUSE_MIDDLE", getMouseButtonFromGlfw(GLFW_MOUSE_BUTTON_MIDDLE)},
    {"MOUSE_MOVE", getMouseMove()},

    // Gamepad button
    {"GAMEPAD_SOUTH", getGamepadButtonFromGlfw(GLFW_GAMEPAD_BUTTON_CROSS)},
    {"GAMEPAD_EAST", getGamepadButtonFromGlfw(GLFW_GAMEPAD_BUTTON_CIRCLE)},
    {"GAMEPAD_WEST", getGamepadButtonFromGlfw(GLFW_GAMEPAD_BUTTON_SQUARE)},
    {"GAMEPAD_NORTH", getGamepadButtonFromGlfw(GLFW_GAMEPAD_BUTTON_TRIANGLE)},

    {"GAMEPAD_BUMPER_LEFT",
     getGamepadButtonFromGlfw(GLFW_GAMEPAD_BUTTON_LEFT_BUMPER)},
    {"GAMEPAD_BUMPER_RIGHT",
     getGamepadButtonFromGlfw(GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER)},

    {"GAMEPAD_THUMB_LEFT",
     getGamepadButtonFromGlfw(GLFW_GAMEPAD_BUTTON_LEFT_THUMB)},
    {"GAMEPAD_THUMB_RIGHT",
     getGamepadButtonFromGlfw(GLFW_GAMEPAD_BUTTON_RIGHT_THUMB)},

    {"GAMEPAD_DPAD_UP", getGamepadButtonFromGlfw(GLFW_GAMEPAD_BUTTON_DPAD_UP)},
    {"GAMEPAD_DPAD_RIGHT",
     getGamepadButtonFromGlfw(GLFW_GAMEPAD_BUTTON_DPAD_RIGHT)},
    {"GAMEPAD_DPAD_DOWN",
     getGamepadButtonFromGlfw(GLFW_GAMEPAD_BUTTON_DPAD_DOWN)},
    {"GAMEPAD_DPAD_LEFT",
     getGamepadButtonFromGlfw(GLFW_GAMEPAD_BUTTON_DPAD_LEFT)},

    {"GAMEPAD_START", getGamepadButtonFromGlfw(GLFW_GAMEPAD_BUTTON_START)},
    {"GAMEPAD_BACK", getGamepadButtonFromGlfw(GLFW_GAMEPAD_BUTTON_BACK)},

    // Gamepad axis
    {"GAMEPAD_LEFT_X", getGamepadAxisFromGlfw(GLFW_GAMEPAD_AXIS_LEFT_X)},
    {"GAMEPAD_LEFT_Y", getGamepadAxisFromGlfw(GLFW_GAMEPAD_AXIS_LEFT_Y)},
    {"GAMEPAD_RIGHT_X", getGamepadAxisFromGlfw(GLFW_GAMEPAD_AXIS_RIGHT_X)},
    {"GAMEPAD_RIGHT_Y", getGamepadAxisFromGlfw(GLFW_GAMEPAD_AXIS_RIGHT_Y)},

    {"GAMEPAD_TRIGGER_LEFT",
     getGamepadAxisFromGlfw(GLFW_GAMEPAD_AXIS_LEFT_TRIGGER)},
    {"GAMEPAD_TRIGGER_RIGHT",
     getGamepadAxisFromGlfw(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER)},

    // End
};

static const std::unordered_map<String, InputDataType> InputDataTypeMap{
    // Mouse
    {"MOUSE_MOVE", InputDataType::Axis2d},

    // Gamepad
    {"GAMEPAD_LEFT_X", InputDataType::Axis1d},
    {"GAMEPAD_LEFT_Y", InputDataType::Axis1d},
    {"GAMEPAD_RIGHT_X", InputDataType::Axis1d},
    {"GAMEPAD_RIGHT_Y", InputDataType::Axis1d}
    // End
};

bool exists(const String &key) { return Mapping.contains(key); }

int get(const String &key) {
  auto it = Mapping.find(key);
  if (it == Mapping.end()) {
    return GLFW_KEY_UNKNOWN;
  }

  return it->second;
}

bool isGamepadButton(int key) {
  return key >= GamepadStart && key < GamepadAxisStart;
}

bool isGamepadAxis(int key) { return key >= GamepadAxisStart; }

bool isMouseMove(int key) { return key == getMouseMove(); }

int getGlfwKeyboardKey(int key) { return key - KeyboardStart; }

int getGlfwMouseButton(int key) { return key - MouseStart; }

int getGlfwGamepadButton(int key) { return key - GamepadStart; }

int getGlfwGamepadAxis(int key) { return key - GamepadAxisStart; }

InputDataType getKeyDataType(const String &key) {
  auto it = InputDataTypeMap.find(key);
  if (it != InputDataTypeMap.end()) {
    return it->second;
  }

  return InputDataType::Boolean;
}

} // namespace quoll::input
