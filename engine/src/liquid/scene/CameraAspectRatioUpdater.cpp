#include "liquid/core/Base.h"
#include "CameraAspectRatioUpdater.h"

namespace liquid {

CameraAspectRatioUpdater::CameraAspectRatioUpdater(Window &window,
                                                   EntityContext &entityContext)
    : mWindow(window), mEntityContext(entityContext) {}

void CameraAspectRatioUpdater::update() {
  LIQUID_PROFILE_EVENT("CameraAspectRatioUpdater::update");
  const auto &size = mWindow.getWindowSize();

  mEntityContext
      .iterateEntities<PerspectiveLensComponent, AutoAspectRatioComponent>(
          [&size](auto entity, auto &lens, auto &_) {
            lens.aspectRatio =
                static_cast<float>(size.x) / static_cast<float>(size.y);
          });
}

} // namespace liquid
