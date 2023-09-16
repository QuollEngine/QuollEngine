#include "quoll/core/Base.h"
#include "KeyMappings.h"

#include <GLFW/glfw3.h>

namespace quoll::input {

static constexpr int MouseStart = 1000;
static constexpr int GamepadStart = 2000;
static constexpr int GamepadAxisStart = 2100;

static const std::unordered_map<String, int> Mapping{
    // letters
    {"KEY_A", GLFW_KEY_A},
    {"KEY_B", GLFW_KEY_B},
    {"KEY_C", GLFW_KEY_C},
    {"KEY_D", GLFW_KEY_D},
    {"KEY_E", GLFW_KEY_E},
    {"KEY_F", GLFW_KEY_F},
    {"KEY_G", GLFW_KEY_G},
    {"KEY_H", GLFW_KEY_H},
    {"KEY_I", GLFW_KEY_I},
    {"KEY_J", GLFW_KEY_J},
    {"KEY_K", GLFW_KEY_K},
    {"KEY_L", GLFW_KEY_L},
    {"KEY_M", GLFW_KEY_M},
    {"KEY_N", GLFW_KEY_N},
    {"KEY_O", GLFW_KEY_O},
    {"KEY_P", GLFW_KEY_P},
    {"KEY_Q", GLFW_KEY_Q},
    {"KEY_R", GLFW_KEY_R},
    {"KEY_S", GLFW_KEY_S},
    {"KEY_T", GLFW_KEY_T},
    {"KEY_U", GLFW_KEY_U},
    {"KEY_V", GLFW_KEY_V},
    {"KEY_W", GLFW_KEY_W},
    {"KEY_X", GLFW_KEY_X},
    {"KEY_Y", GLFW_KEY_Y},
    {"KEY_Z", GLFW_KEY_Z},

    // digits
    {"KEY_1", GLFW_KEY_1},
    {"KEY_2", GLFW_KEY_2},
    {"KEY_3", GLFW_KEY_3},
    {"KEY_4", GLFW_KEY_4},
    {"KEY_5", GLFW_KEY_5},
    {"KEY_6", GLFW_KEY_6},
    {"KEY_7", GLFW_KEY_7},
    {"KEY_8", GLFW_KEY_8},
    {"KEY_9", GLFW_KEY_9},

    // special
    {"KEY_comma", GLFW_KEY_COMMA},
    {"KEY_minus", GLFW_KEY_MINUS},
    {"KEY_period", GLFW_KEY_PERIOD},
    {"KEY_slash", GLFW_KEY_SLASH},
    {"KEY_semicolon", GLFW_KEY_SEMICOLON},
    {"KEY_equal", GLFW_KEY_EQUAL},
    {"KEY_left_bracket", GLFW_KEY_LEFT_BRACKET},
    {"KEY_backslash", GLFW_KEY_BACKSLASH},
    {"KEY_right_bracket", GLFW_KEY_RIGHT_BRACKET},
    {"KEY_grave_accent", GLFW_KEY_GRAVE_ACCENT},

    // command keys
    {"KEY_SPACE", GLFW_KEY_SPACE},
    {"KEY_ESCAPE", GLFW_KEY_ESCAPE},
    {"KEY_ENTER", GLFW_KEY_ENTER},
    {"KEY_TAB", GLFW_KEY_TAB},
    {"KEY_BACKSPACE", GLFW_KEY_BACKSPACE},
    {"KEY_INSERT", GLFW_KEY_INSERT},
    {"KEY_DELETE", GLFW_KEY_DELETE},
    {"KEY_RIGHT", GLFW_KEY_RIGHT},
    {"KEY_LEFT", GLFW_KEY_LEFT},
    {"KEY_DOWN", GLFW_KEY_DOWN},
    {"KEY_UP", GLFW_KEY_UP},
    {"KEY_PAGE_UP", GLFW_KEY_PAGE_UP},
    {"KEY_PAGE_DOWN", GLFW_KEY_PAGE_DOWN},
    {"KEY_HOME", GLFW_KEY_HOME},
    {"KEY_END", GLFW_KEY_END},
    {"KEY_CAPS_LOCK", GLFW_KEY_CAPS_LOCK},
    {"KEY_SCROLL_LOCK", GLFW_KEY_SCROLL_LOCK},
    {"KEY_NUM_LOCK", GLFW_KEY_NUM_LOCK},
    {"KEY_PRINT_SCREEN", GLFW_KEY_PRINT_SCREEN},
    {"KEY_PAUSE", GLFW_KEY_PAUSE},
    {"KEY_LEFT_SHIFT", GLFW_KEY_LEFT_SHIFT},
    {"KEY_LEFT_CONTROL", GLFW_KEY_LEFT_CONTROL},
    {"KEY_LEFT_ALT", GLFW_KEY_LEFT_ALT},
    {"KEY_LEFT_SUPER", GLFW_KEY_LEFT_SUPER},
    {"KEY_RIGHT_SHIFT", GLFW_KEY_RIGHT_SHIFT},
    {"KEY_RIGHT_CONTROL", GLFW_KEY_RIGHT_CONTROL},
    {"KEY_RIGHT_ALT", GLFW_KEY_RIGHT_ALT},
    {"KEY_RIGHT_SUPER", GLFW_KEY_RIGHT_SUPER},

    // Mouse
    {"MOUSE_LEFT", MouseStart + GLFW_MOUSE_BUTTON_LEFT},
    {"MOUSE_RIGHT", MouseStart + GLFW_MOUSE_BUTTON_RIGHT},
    {"MOUSE_MIDDLE", MouseStart + GLFW_MOUSE_BUTTON_MIDDLE},
    {"MOUSE_MOVE", MouseStart + 10},

    // Gamepad
    {"GAMEPAD_SOUTH", GamepadStart + GLFW_GAMEPAD_BUTTON_CROSS},
    {"GAMEPAD_EAST", GamepadStart + GLFW_GAMEPAD_BUTTON_CIRCLE},
    {"GAMEPAD_WEST", GamepadStart + GLFW_GAMEPAD_BUTTON_SQUARE},
    {"GAMEPAD_NORTH", GamepadStart + GLFW_GAMEPAD_BUTTON_TRIANGLE},

    {"GAMEPAD_BUMPER_LEFT", GamepadStart + GLFW_GAMEPAD_BUTTON_LEFT_BUMPER},
    {"GAMEPAD_BUMPER_RIGHT", GamepadStart + GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER},

    {"GAMEPAD_THUMB_LEFT", GamepadStart + GLFW_GAMEPAD_BUTTON_LEFT_THUMB},
    {"GAMEPAD_THUMB_RIGHT", GamepadStart + GLFW_GAMEPAD_BUTTON_RIGHT_THUMB},

    {"GAMEPAD_DPAD_UP", GamepadStart + GLFW_GAMEPAD_BUTTON_DPAD_UP},
    {"GAMEPAD_DPAD_RIGHT", GamepadStart + GLFW_GAMEPAD_BUTTON_DPAD_RIGHT},
    {"GAMEPAD_DPAD_DOWN", GamepadStart + GLFW_GAMEPAD_BUTTON_DPAD_DOWN},
    {"GAMEPAD_DPAD_LEFT", GamepadStart + GLFW_GAMEPAD_BUTTON_DPAD_LEFT},

    {"GAMEPAD_START", GamepadStart + GLFW_GAMEPAD_BUTTON_START},
    {"GAMEPAD_BACK", GamepadStart + GLFW_GAMEPAD_BUTTON_BACK},

    {"GAMEPAD_LEFT_X", GamepadAxisStart + GLFW_GAMEPAD_AXIS_LEFT_X},
    {"GAMEPAD_LEFT_Y", GamepadAxisStart + GLFW_GAMEPAD_AXIS_LEFT_Y},
    {"GAMEPAD_RIGHT_X", GamepadAxisStart + GLFW_GAMEPAD_AXIS_RIGHT_Y},
    {"GAMEPAD_RIGHT_Y", GamepadAxisStart + GLFW_GAMEPAD_AXIS_RIGHT_Y},

    {"GAMEPAD_TRIGGER_LEFT", GamepadAxisStart + GLFW_GAMEPAD_AXIS_LEFT_TRIGGER},
    {"GAMEPAD_TRIGGER_RIGHT",
     GamepadAxisStart + GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER},

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

InputDataType getKeyDataType(const String &key) {
  auto it = InputDataTypeMap.find(key);
  if (it != InputDataTypeMap.end()) {
    return it->second;
  }

  return InputDataType::Boolean;
}

} // namespace quoll::input
