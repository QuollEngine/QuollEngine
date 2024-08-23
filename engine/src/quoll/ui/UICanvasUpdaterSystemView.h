#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "UICanvas.h"
#include "UICanvasRenderRequest.h"

namespace quoll {

struct UICanvasUpdaterSystemView {
  flecs::query<UICanvas> queryCanvases;
  flecs::query<UICanvas, UICanvasRenderRequest> queryRenderRequests;
};

} // namespace quoll
