#pragma once

#include "liquid/rhi/RenderGraph.h"
#include "liquid/rhi/RenderDevice.h"
#include "liquid/rhi/RenderGraphEvaluator.h"
#include "liquid/renderer/ShaderLibrary.h"
#include "liquid/renderer/RenderStorage.h"
#include "liquid/asset/AssetRegistry.h"

#include "liquid/window/Window.h"

namespace liquidator {

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
   * @param shaderLibrary Shader library
   * @param renderStorage Render storage
   * @param assetRegistry Asset registry
   * @param device Render device
   */
  MousePickingGraph(liquid::ShaderLibrary &shaderLibrary,
                    const liquid::RenderStorage &renderStorage,
                    liquid::AssetRegistry &assetRegistry,
                    liquid::rhi::RenderDevice *device);

  ~MousePickingGraph();

  MousePickingGraph(const MousePickingGraph &) = delete;
  MousePickingGraph &operator=(const MousePickingGraph &) = delete;
  MousePickingGraph(MousePickingGraph &&) = delete;
  MousePickingGraph &operator=(MousePickingGraph &&) = delete;

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
  void execute(liquid::rhi::RenderCommandList &commandList,
               const glm::vec2 &mousePos, uint32_t frameIndex);

  /**
   * @brief Get selected entity
   *
   * If no entity is selected,
   * returns null entity
   *
   * @return Selected entity
   */
  liquid::Entity getSelectedEntity();

  /**
   * @brief Set framebuffer size
   *
   * @param window Framebuffer size
   */
  void setFramebufferSize(liquid::Window &window);

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
  liquid::rhi::RenderDevice *mDevice = nullptr;

  liquid::rhi::RenderGraph mRenderGraph;
  liquid::rhi::RenderGraphEvaluator mGraphEvaluator;
  const liquid::RenderStorage &mRenderStorage;
  liquid::AssetRegistry &mAssetRegistry;

  liquid::rhi::Buffer mEntitiesBuffer;
  liquid::rhi::Buffer mSkinnedEntitiesBuffer;
  liquid::rhi::Buffer mSelectedEntityBuffer;

  glm::vec2 mMousePos{};

  uint32_t mFrameIndex = std::numeric_limits<uint32_t>::max();
};

} // namespace liquidator
