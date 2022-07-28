#pragma once

namespace liquidator {

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

} // namespace liquidator
