#pragma once

namespace quoll::editor {

class Shortcut {
public:
  Shortcut() = default;

  Shortcut(int key, int mods);

  Shortcut &shift();

  Shortcut &control();

  Shortcut &alt();

  Shortcut &key(char key);

  inline int getMods() const { return mMods; }

  inline int getKey() const { return mKey; }

  inline operator bool() const { return mKey != -1 && mMods != 0; }

  String toString() const;

  bool operator==(const Shortcut &rhs) const;

private:
  int mMods = 0;
  int mKey = -1;
};

} // namespace quoll::editor
