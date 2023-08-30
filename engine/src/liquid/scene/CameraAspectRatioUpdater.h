#pragma once

#include "liquid/window/Window.h"
#include "liquid/entity/EntityDatabase.h"

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
   * @brief Create camera aspect ratio updater
   *
   * @param window Window
   */
  CameraAspectRatioUpdater(Window &window);

  /**
   * @brief Update aspect ratios
   *
   * @param entityDatabase Entity database
   */
  void update(EntityDatabase &entityDatabase);

private:
  Window &mWindow;
};

} // namespace quoll
