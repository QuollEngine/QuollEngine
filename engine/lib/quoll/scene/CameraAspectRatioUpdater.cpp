#include "quoll/core/Base.h"
#include "quoll/core/Profiler.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/system/SystemView.h"
#include "AutoAspectRatio.h"
#include "CameraAspectRatioUpdater.h"
#include "PerspectiveLens.h"

namespace quoll {

void CameraAspectRatioUpdater::update(SystemView &view) const {
  auto &entityDatabase = view.scene->entityDatabase;
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
