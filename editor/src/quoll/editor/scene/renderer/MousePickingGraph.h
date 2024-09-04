#pragma once

#include "quoll/asset/AssetRegistry.h"
#include "quoll/renderer/RenderGraph.h"
#include "quoll/renderer/SceneRendererFrameData.h"
#include "quoll/window/Window.h"

namespace quoll {

class RendererAssetRegistry;

} // namespace quoll

namespace quoll::editor {

class MousePickingGraph {
  struct MousePickingFrameData {
    std::vector<glm::vec4> textBounds;
  };

public:
  MousePickingGraph(const std::array<SceneRendererFrameData, 2> &frameData,
                    RenderStorage &renderStorage,
                    RendererAssetRegistry &rendererAssetRegistry);

  void execute(rhi::RenderCommandList &commandList, const glm::vec2 &mousePos,
               u32 frameIndex);

  Entity getSelectedEntity();

  void setFramebufferSize(glm::uvec2 size);

  inline bool isSelectionPerformedInFrame(u32 frameIndex) const {
    return frameIndex == mFrameIndex;
  }

  void recreateIfResized();

private:
  void createRenderGraph();

private:
  RenderStorage &mRenderStorage;
  RendererAssetRegistry &mRendererAssetRegistry;
  RenderGraph mRenderGraph;
  const std::array<SceneRendererFrameData, rhi::RenderDevice::NumFrames>
      &mFrameData;
  std::array<BindlessDrawParameters, rhi::RenderDevice::NumFrames>
      mBindlessParams;

  std::array<MousePickingFrameData, rhi::RenderDevice::NumFrames>
      mMousePickingFrameData;

  rhi::Buffer mSpriteEntitiesBuffer;
  rhi::Buffer mMeshEntitiesBuffer;
  rhi::Buffer mSkinnedMeshEntitiesBuffer;
  rhi::Buffer mTextEntitiesBuffer;
  rhi::Buffer mSelectedEntityBuffer;

  glm::vec2 mMousePos{};

  glm::uvec2 mFramebufferSize{};
  bool mResized = true;

  u32 mFrameIndex = std::numeric_limits<u32>::max();
};

} // namespace quoll::editor
