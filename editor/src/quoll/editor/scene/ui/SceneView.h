#pragma once

#include "quoll/rhi/RenderHandle.h"

namespace quoll::editor {

class SceneView {
public:
  SceneView(rhi::TextureHandle texture);

  ~SceneView();

  SceneView(const SceneView &) = delete;
  SceneView(SceneView &&) = delete;
  SceneView &operator=(const SceneView &) = delete;
  SceneView &operator=(SceneView &&) = delete;

  inline operator bool() const { return mExpanded; }

private:
  bool mExpanded = false;
};

} // namespace quoll::editor
