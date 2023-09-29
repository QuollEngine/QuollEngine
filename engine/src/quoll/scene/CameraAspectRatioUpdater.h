#pragma once

#include "quoll/entity/EntityDatabase.h"

namespace quoll {

/**
 * @brief Camera aspect ratio updater
 *
 * Updates camera with auto aspect ratio
 * based on Window aspect ratio
 */
class CameraAspectRatioUpdater {
public:
  /**
   * @brief Update aspect ratios
   *
   * @param entityDatabase Entity database
   */
  void update(EntityDatabase &entityDatabase);

  /**
   * @brief Set viewport size
   *
   * @param size Viewport size
   */
  void setViewportSize(glm::uvec2 size);

private:
  glm::uvec2 mSize{};
};

} // namespace quoll
