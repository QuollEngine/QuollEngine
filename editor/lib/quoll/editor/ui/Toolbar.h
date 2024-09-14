#pragma once

namespace quoll::editor {

class Toolbar : NoCopyMove {
public:
  static constexpr f32 Height = 60.0f;

public:
  Toolbar();

  ~Toolbar();

  bool item(String label, String icon, bool active);

  inline operator bool() { return mOpen; }

private:
  bool mOpen = false;
};

} // namespace quoll::editor
