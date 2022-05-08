#pragma once

#include "liquid/window/Window.h"
#include "liquid/entity/EntityContext.h"

namespace liquid {

class CameraAspectRatioUpdater {
public:
  CameraAspectRatioUpdater(Window &window, EntityContext &entityContext);

  void update();

private:
  Window &mWindow;
  EntityContext &mEntityContext;
};

} // namespace liquid
