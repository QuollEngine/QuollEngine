#pragma once

#include "liquid/window/Window.h"
#include "liquid/entity/EntityContext.h"

namespace liquid {

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
