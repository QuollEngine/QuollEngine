#pragma once

#include "quoll/renderer/RenderGraph.h"
#include "quoll/renderer/SceneRendererFrameData.h"
#include "quoll/asset/AssetRegistry.h"

#include "quoll/window/Window.h"

namespace quoll::editor {

/**
 * @brief Render graph for mouse picking
 *
 * Performs mouse picking using shaders
 * and writes the output to a host visible
 * storage buffer
 */
class MousePickingGraph {
  struct MousePickingFrameData {
    std::vector<glm::vec4> textBounds;
  };

public:
  /**
   * @brief Create mouse picking graph
   *
   * @param frameData Scene renderer frame data
   * @param assetRegistry Asset registry
   * @param renderStorage Render storage
   */
  MousePickingGraph(const std::array<SceneRendererFrameData, 2> &frameData,
                    AssetRegistry &assetRegistry, RenderStorage &renderStorage);

  /**
   * @brief Execute the graph
   *
   * @param commandList Command list
   * @param mousePos Mouse position
   * @param frameIndex Frame index
   */
  void execute(rhi::RenderCommandList &commandList, const glm::vec2 &mousePos,
               u32 frameIndex);

  /**
   * @brief Get selected entity
   *
   * If no entity is selected,
   * returns null entity
   *
   * @return Selected entity
   */
  Entity getSelectedEntity();

  /**
   * @brief Set framebuffer size
   *
   * @param size Framebuffer size
   */
  void setFramebufferSize(glm::uvec2 size);

  /**
   * @brief Check is selection is performed in current frame
   *
   * @param frameIndex Frame index
   * @retval true Selection is performed in current frame
   * @retval false Selection is not performed in current frame
   */
  inline bool isSelectionPerformedInFrame(u32 frameIndex) const {
    return frameIndex == mFrameIndex;
  }

private:
  /**
   * @brief Create render graph
   */
  void createRenderGraph();

private:
  RenderStorage &mRenderStorage;
  RenderGraph mRenderGraph;
  const std::array<SceneRendererFrameData, rhi::RenderDevice::NumFrames>
      &mFrameData;
  std::array<BindlessDrawParameters, rhi::RenderDevice::NumFrames>
      mBindlessParams;

  std::array<MousePickingFrameData, rhi::RenderDevice::NumFrames>
      mMousePickingFrameData;

  AssetRegistry &mAssetRegistry;

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
