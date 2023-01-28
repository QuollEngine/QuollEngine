#pragma once

namespace liquid::editor {

/**
 * @brief Docking layout component
 */
class Layout {
public:
  /**
   * Set up the docking layout
   */
  void setup();

private:
  bool mFirstTime = true;
};

} // namespace liquid::editor
