#pragma once

#include "quoll/rhi/RenderHandle.h"

namespace quoll::editor {

class SceneView : NoCopyMove {
public:
  SceneView(rhi::TextureHandle texture);

  ~SceneView();

  inline operator bool() const { return mExpanded; }

private:
  bool mExpanded = false;
};

} // namespace quoll::editor
