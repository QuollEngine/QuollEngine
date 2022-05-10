#pragma once

#include "liquid/window/Window.h"
#include "liquid/entity/EntityContext.h"

namespace liquid {

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
   * @param entityContext Entity context
   */
  void update(EntityContext &entityContext);

private:
  Window &mWindow;
};

} // namespace liquid
