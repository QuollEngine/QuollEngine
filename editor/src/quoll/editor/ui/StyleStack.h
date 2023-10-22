#pragma once

#include "quoll/imgui/Imgui.h"

namespace quoll::editor {

/**
 * @brief Style stack
 *
 * Automatically clears the Imgui stack
 * on destruct
 */
class StyleStack {
public:
  /**
   * @brief Default constructor
   */
  StyleStack() = default;

  /**
   * @brief Pop stack values
   */
  ~StyleStack();

  StyleStack(const StyleStack &) = delete;
  StyleStack(StyleStack &&) = delete;
  StyleStack &operator=(const StyleStack &) = delete;
  StyleStack &operator=(StyleStack &&) = delete;

  /**
   * @brief Push color to the stack
   *
   * @param colorIndex Color index
   * @param color Color value
   */
  void pushColor(u32 colorIndex, const glm::vec4 &color);

  /**
   * @brief Push color to the stack
   *
   * @param colorIndex Color index
   * @param color Color value
   */
  void pushColor(u32 colorIndex, const ImVec4 &color);

  /**
   * @brief Push style to the stack
   *
   * @param styleIndex Style index
   * @param value Style value
   */
  void pushStyle(u32 styleIndex, f32 value);

  /**
   * @brief Push style to the stack
   *
   * @param styleIndex Style index
   * @param value Style value
   */
  void pushStyle(u32 styleIndex, const glm::vec2 &value);

  /**
   * @brief Push style to the stack
   *
   * @param styleIndex Style index
   * @param value Style value
   */
  void pushStyle(u32 styleIndex, const ImVec2 &value);

  /**
   * @brief Push style to the stack
   *
   * @param font Font
   */
  void pushFont(ImFont *font);

private:
  u32 mPushedColors = 0;
  u32 mPushedStyles = 0;
  u32 mPushedFonts = 0;
};

} // namespace quoll::editor
