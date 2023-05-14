#pragma once

#include "liquid/renderer/RenderGraph.h"
#include "liquid/renderer/SceneRendererFrameData.h"
#include "liquid/asset/AssetRegistry.h"

#include "liquid/window/Window.h"

namespace liquid::editor {

/**
 * @brief Render graph for mouse picking
 *
 * Performs mouse picking using shaders
 * and writes the output to a host visible
 * storage buffer
 */
class MousePickingGraph {
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
   * @brief Compile the graph
   */
  void compile();

  /**
   * @brief Execute the graph
   *
   * @param commandList Command list
   * @param mousePos Mouse position
   * @param frameIndex Frame index
   */
  void execute(rhi::RenderCommandList &commandList, const glm::vec2 &mousePos,
               uint32_t frameIndex);

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
   * @param window Framebuffer size
   */
  void setFramebufferSize(Window &window);

  /**
   * @brief Check is selection is performed in current frame
   *
   * @param frameIndex Frame index
   * @retval true Selection is performed in current frame
   * @retval false Selection is not performed in current frame
   */
  inline bool isSelectionPerformedInFrame(uint32_t frameIndex) const {
    return frameIndex == mFrameIndex;
  }

private:
  RenderStorage &mRenderStorage;
  RenderGraph mRenderGraph;
  const std::array<SceneRendererFrameData, rhi::RenderDevice::NumFrames>
      &mFrameData;
  std::array<BindlessDrawParameters, rhi::RenderDevice::NumFrames>
      mBindlessParams;

  AssetRegistry &mAssetRegistry;

  rhi::Buffer mSpriteEntitiesBuffer;
  rhi::Buffer mMeshEntitiesBuffer;
  rhi::Buffer mSkinnedMeshEntitiesBuffer;
  rhi::Buffer mSelectedEntityBuffer;

  glm::vec2 mMousePos{};

  uint32_t mFrameIndex = std::numeric_limits<uint32_t>::max();
};

} // namespace liquid::editor
