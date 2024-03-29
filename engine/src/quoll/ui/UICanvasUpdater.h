#pragma once

#include "quoll/asset/AssetRegistry.h"
#include "quoll/entity/EntityDatabase.h"

namespace quoll {

class UICanvasUpdater {
public:
  void render(EntityDatabase &entityDatabase, AssetRegistry &assetRegistry);

  void setViewport(f32 x, f32 y, f32 width, f32 height);

private:
  glm::vec2 mPosition;
  glm::vec2 mSize;
  bool mViewportChanged = false;
};

} // namespace quoll
