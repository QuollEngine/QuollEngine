#pragma once

namespace liquid {

/**
 * @brief Debug manager
 */
class DebugManager {
public:
  /**
   * @brief Set wireframe mode
   *
   * @param wireframeMode Wireframe mode state
   */
  void setWireframeMode(bool wireframeMode);

  /**
   * @brief Get wireframe mode
   *
   * @return Wireframe mode state
   */
  inline bool getWireframeMode() const { return mWireframeMode; }

private:
  bool mWireframeMode = false;
};

} // namespace liquid
