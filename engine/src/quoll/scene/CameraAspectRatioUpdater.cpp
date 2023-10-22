#include "quoll/core/Base.h"
#include "CameraAspectRatioUpdater.h"

namespace quoll {

void CameraAspectRatioUpdater::update(EntityDatabase &entityDatabase) {
  QUOLL_PROFILE_EVENT("CameraAspectRatioUpdater::update");

  if (mSize.x <= 0 || mSize.y <= 0)
    return;

  for (auto [_, lens, _1] :
       entityDatabase.view<PerspectiveLens, AutoAspectRatio>()) {
    lens.aspectRatio = static_cast<f32>(mSize.x) / static_cast<f32>(mSize.y);
  }
}

void CameraAspectRatioUpdater::setViewportSize(glm::uvec2 size) {
  mSize = size;
}

} // namespace quoll
