#pragma once

#include "InputDataType.h"

namespace quoll::input {

bool exists(const String &key);

int get(const String &key);

bool isGamepadButton(int key);

bool isGamepadAxis(int key);

bool isMouseMove(int key);

int getGlfwKeyboardKey(int key);

int getGlfwMouseButton(int key);

int getGlfwGamepadButton(int key);

int getGlfwGamepadAxis(int key);

InputDataType getKeyDataType(const String &key);

} // namespace quoll::input
