#pragma once

namespace liquidator {

/**
 * @brief Inspector panel component
 */
class Inspector {
public:
  /**
   * Render inspector panel
   *
   * @param fn Child renderer
   */
  void render(std::function<void()> &&fn);

private:
  bool mOpen = false;
};

} // namespace liquidator
