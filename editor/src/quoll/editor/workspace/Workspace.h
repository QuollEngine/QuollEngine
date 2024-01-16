#pragma once

#include "quoll/rhi/RenderCommandList.h"

#include "WorkspaceMatchParams.h"

namespace quoll::editor {

class Workspace {
public:
  Workspace() = default;

  virtual ~Workspace() = default;

  Workspace(const Workspace &) = delete;
  Workspace &operator=(const Workspace &) = delete;
  Workspace(Workspace &&) = delete;
  Workspace &operator=(Workspace &&) = delete;

  virtual void prepare() = 0;

  virtual void fixedUpdate(f32 dt) = 0;

  virtual void update(f32 dt) = 0;

  virtual void render() = 0;

  virtual void processShortcuts(int key, int mods) = 0;

  virtual void updateFrameData(rhi::RenderCommandList &commandList,
                               u32 frameIndex) = 0;

  virtual WorkspaceMatchParams getMatchParams() const = 0;

  virtual void reload() = 0;
};

} // namespace quoll::editor
