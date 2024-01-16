#pragma once

namespace quoll::editor {

class Toolbar {
public:
  static constexpr f32 Height = 60.0f;

public:
  Toolbar();

  ~Toolbar();

  bool item(String label, String icon, bool active);

  inline operator bool() { return mOpen; }

  Toolbar(const Toolbar &) = delete;
  Toolbar(Toolbar &&) = delete;
  Toolbar &operator=(const Toolbar &) = delete;
  Toolbar &operator=(Toolbar &&) = delete;

private:
  bool mOpen = false;
};

} // namespace quoll::editor
