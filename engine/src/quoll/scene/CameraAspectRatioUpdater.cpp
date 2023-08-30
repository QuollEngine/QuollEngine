#include "quoll/core/Base.h"
#include "CameraAspectRatioUpdater.h"

namespace quoll {

CameraAspectRatioUpdater::CameraAspectRatioUpdater(Window &window)
    : mWindow(window) {}

void CameraAspectRatioUpdater::update(EntityDatabase &entityDatabase) {
  LIQUID_PROFILE_EVENT("CameraAspectRatioUpdater::update");
  const auto &size = mWindow.getWindowSize();

  if (size.x <= 0 || size.y <= 0)
    return;

  for (auto [_, lens, _1] :
       entityDatabase.view<PerspectiveLens, AutoAspectRatio>()) {
    lens.aspectRatio = static_cast<float>(size.x) / static_cast<float>(size.y);
  }
}

} // namespace quoll
