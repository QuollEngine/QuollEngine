#pragma once

#include "quoll/imgui/Imgui.h"

namespace quoll::editor {

class StyleStack : NoCopyMove {
public:
  StyleStack() = default;

  ~StyleStack();

  void pushColor(u32 colorIndex, const glm::vec4 &color);

  void pushColor(u32 colorIndex, const ImVec4 &color);

  void pushStyle(u32 styleIndex, f32 value);

  void pushStyle(u32 styleIndex, const glm::vec2 &value);

  void pushStyle(u32 styleIndex, const ImVec2 &value);

  void pushFont(ImFont *font);

private:
  u32 mPushedColors = 0;
  u32 mPushedStyles = 0;
  u32 mPushedFonts = 0;
};

} // namespace quoll::editor
