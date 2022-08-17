#pragma once

namespace liquidator {

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

private:
  uint32_t mPushedColors = 0;
};

} // namespace liquidator
