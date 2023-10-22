#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "quoll/asset/AssetRegistry.h"

namespace quoll {

/**
 * @brief UI Canvas updater
 */
class UICanvasUpdater {
public:
  /**
   * @brief Render ui canvas components
   *
   * @param entityDatabase Entity database
   * @param assetRegistry Asset registry
   */
  void render(EntityDatabase &entityDatabase, AssetRegistry &assetRegistry);

  /**
   * @brief Set viewport
   *
   * @param x X position
   * @param y Y position
   * @param width Width
   * @param height Height
   */
  void setViewport(f32 x, f32 y, f32 width, f32 height);

private:
  glm::vec2 mPosition;
  glm::vec2 mSize;
  bool mViewportChanged = false;
};

} // namespace quoll
