#include "quoll/core/Base.h"
#include "KeyMappings.h"

#include <GLFW/glfw3.h>

namespace quoll::input {

static const std::unordered_map<String, int> keyboard{
    // letters
    {"A", GLFW_KEY_A},
    {"B", GLFW_KEY_B},
    {"C", GLFW_KEY_C},
    {"D", GLFW_KEY_D},
    {"E", GLFW_KEY_E},
    {"F", GLFW_KEY_F},
    {"G", GLFW_KEY_G},
    {"H", GLFW_KEY_H},
    {"I", GLFW_KEY_I},
    {"J", GLFW_KEY_J},
    {"K", GLFW_KEY_K},
    {"L", GLFW_KEY_L},
    {"M", GLFW_KEY_M},
    {"N", GLFW_KEY_N},
    {"O", GLFW_KEY_O},
    {"P", GLFW_KEY_P},
    {"Q", GLFW_KEY_Q},
    {"R", GLFW_KEY_R},
    {"S", GLFW_KEY_S},
    {"T", GLFW_KEY_T},
    {"U", GLFW_KEY_U},
    {"V", GLFW_KEY_V},
    {"W", GLFW_KEY_W},
    {"X", GLFW_KEY_X},
    {"Y", GLFW_KEY_Y},
    {"Z", GLFW_KEY_Z},

    // digits
    {"1", GLFW_KEY_1},
    {"2", GLFW_KEY_2},
    {"3", GLFW_KEY_3},
    {"4", GLFW_KEY_4},
    {"5", GLFW_KEY_5},
    {"6", GLFW_KEY_6},
    {"7", GLFW_KEY_7},
    {"8", GLFW_KEY_8},
    {"9", GLFW_KEY_9},

    // special
    {"'", GLFW_KEY_COMMA},
    {"-", GLFW_KEY_MINUS},
    {".", GLFW_KEY_PERIOD},
    {"/", GLFW_KEY_SLASH},
    {";", GLFW_KEY_SEMICOLON},
    {"=", GLFW_KEY_EQUAL},
    {"[", GLFW_KEY_LEFT_BRACKET},
    {"\\", GLFW_KEY_BACKSLASH},
    {"]", GLFW_KEY_RIGHT_BRACKET},
    {"`", GLFW_KEY_GRAVE_ACCENT},

    // command keys
    {"SPACE", GLFW_KEY_SPACE},
    {"ESCAPE", GLFW_KEY_ESCAPE},
    {"ENTER", GLFW_KEY_ENTER},
    {"TAB", GLFW_KEY_TAB},
    {"BACKSPACE", GLFW_KEY_BACKSPACE},
    {"INSERT", GLFW_KEY_INSERT},
    {"DELETE", GLFW_KEY_DELETE},
    {"RIGHT", GLFW_KEY_RIGHT},
    {"LEFT", GLFW_KEY_LEFT},
    {"DOWN", GLFW_KEY_DOWN},
    {"UP", GLFW_KEY_UP},
    {"PAGE_UP", GLFW_KEY_PAGE_UP},
    {"PAGE_DOWN", GLFW_KEY_PAGE_DOWN},
    {"HOME", GLFW_KEY_HOME},
    {"END", GLFW_KEY_END},
    {"CAPS_LOCK", GLFW_KEY_CAPS_LOCK},
    {"SCROLL_LOCK", GLFW_KEY_SCROLL_LOCK},
    {"NUM_LOCK", GLFW_KEY_NUM_LOCK},
    {"PRINT_SCREEN", GLFW_KEY_PRINT_SCREEN},
    {"PAUSE", GLFW_KEY_PAUSE},
    {"LEFT_SHIFT", GLFW_KEY_LEFT_SHIFT},
    {"LEFT_CONTROL", GLFW_KEY_LEFT_CONTROL},
    {"LEFT_ALT", GLFW_KEY_LEFT_ALT},
    {"LEFT_SUPER", GLFW_KEY_LEFT_SUPER},
    {"RIGHT_SHIFT", GLFW_KEY_RIGHT_SHIFT},
    {"RIGHT_CONTROL", GLFW_KEY_RIGHT_CONTROL},
    {"RIGHT_ALT", GLFW_KEY_RIGHT_ALT},
    {"RIGHT_SUPER", GLFW_KEY_RIGHT_SUPER}
    // End
};

bool exists(const String &key) { return keyboard.contains(key); }

int get(const String &key) {
  auto it = keyboard.find(key);
  if (it == keyboard.end()) {
    return GLFW_KEY_UNKNOWN;
  }

  return it->second;
}

} // namespace quoll::input
