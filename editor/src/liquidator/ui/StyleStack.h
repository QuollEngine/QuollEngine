#pragma once

#include "liquid/imgui/Imgui.h"

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
  void pushColor(uint32_t colorIndex, const glm::vec4 &color);

  /**
   * @brief Push color to the stack
   *
   * @param colorIndex Color index
   * @param color Color value
   */
  void pushColor(uint32_t colorIndex, const ImVec4 &color);

  /**
   * @brief Push style to the stack
   *
   * @param styleIndex Style index
   * @param value Style value
   */
  void pushStyle(uint32_t styleIndex, float value);

  /**
   * @brief Push style to the stack
   *
   * @param styleIndex Style index
   * @param value Style value
   */
  void pushStyle(uint32_t styleIndex, const glm::vec2 &value);

  /**
   * @brief Push style to the stack
   *
   * @param styleIndex Style index
   * @param value Style value
   */
  void pushStyle(uint32_t styleIndex, const ImVec2 &value);

  /**
   * @brief Push style to the stack
   *
   * @param font Font
   */
  void pushFont(ImFont *font);

private:
  uint32_t mPushedColors = 0;
  uint32_t mPushedStyles = 0;
  uint32_t mPushedFonts = 0;
};

} // namespace quoll::editor
