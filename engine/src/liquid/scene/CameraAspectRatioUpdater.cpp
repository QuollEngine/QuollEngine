#include "liquid/core/Base.h"
#include "CameraAspectRatioUpdater.h"

namespace liquid {

CameraAspectRatioUpdater::CameraAspectRatioUpdater(Window &window)
    : mWindow(window) {}

void CameraAspectRatioUpdater::update(EntityDatabase &entityDatabase) {
  LIQUID_PROFILE_EVENT("CameraAspectRatioUpdater::update");
  const auto &size = mWindow.getWindowSize();

  if (size.x <= 0 || size.y <= 0)
    return;

  entityDatabase
      .iterateEntities<PerspectiveLensComponent, AutoAspectRatioComponent>(
          [&size](auto entity, auto &lens, auto &_) {
            lens.aspectRatio =
                static_cast<float>(size.x) / static_cast<float>(size.y);
          });
}

} // namespace liquid
