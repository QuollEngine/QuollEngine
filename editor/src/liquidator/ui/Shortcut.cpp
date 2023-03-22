#include "liquid/core/Base.h"
#include "Shortcut.h"

#include <GLFW/glfw3.h>

namespace liquid::editor {

Shortcut::Shortcut(int key, int mods) : mKey(key), mMods(mods) {}

Shortcut &Shortcut::shift() {
  mMods |= GLFW_MOD_SHIFT;
  return *this;
}

Shortcut &Shortcut::control() {
  mMods |= GLFW_MOD_CONTROL;
  return *this;
}

Shortcut &Shortcut::alt() {
  mMods |= GLFW_MOD_ALT;
  return *this;
}

Shortcut &Shortcut::key(char key) {
  mKey = key;
  return *this;
}

String Shortcut::toString() const {
  String name;
  String delimiter = "";

  if ((mMods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL) {
    name += "Ctrl";
    delimiter = " + ";
  }

  if ((mMods & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT) {
    name += delimiter + "Shift";
    delimiter = " + ";
  }

  if ((mMods & GLFW_MOD_ALT) == GLFW_MOD_ALT) {
    name += delimiter + "Alt";

    delimiter = " + ";
  }

  if (mKey > 0) {
    name += delimiter + static_cast<char>(mKey);
  }

  return name;
}

bool Shortcut::operator==(const Shortcut &rhs) const {
  return mKey == rhs.mKey && mMods == rhs.mMods;
}

} // namespace liquid::editor
