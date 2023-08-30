#pragma once

#include "quoll/rhi/RenderDevice.h"
#include "quoll/entity/EntityDatabase.h"
#include "RendererOptions.h"
#include "RenderGraph.h"

namespace quoll {

class Window;

/**
 * @brief Default graph resources
 */
struct DefaultGraphResources {
  /**
   * Scene color texture
   */
  rhi::TextureHandle sceneColor;

  /**
   * Depth buffer texture
   */
  rhi::TextureHandle depthBuffer;

  /**
   * Imgui texture
   */
  rhi::TextureHandle imguiColor;

  /**
   * Default color
   */
  glm::vec4 defaultColor;
};

/**
 * @brief Renderer textures
 */
struct RendererTextures {
  /**
   * Final texture
   */
  RenderGraphResource<rhi::TextureHandle> finalTexture;

  /**
   * Scene texture
   */
  RenderGraphResource<rhi::TextureHandle> sceneTexture;
};

/**
 * @brief Renderer
 *
 * Executes the graph in the render device
 */
class Renderer {
public:
  /**
   * @brief Renderer graph builder function type
   */
  using GraphBuilderFn =
      std::function<RendererTextures(RenderGraph &, const RendererOptions &)>;

public:
  /**
   * @brief Create renderer
   *
   * @param storage Render storage
   * @param options Renderer options
   */
  Renderer(RenderStorage &storage, const RendererOptions &options);

  ~Renderer() = default;
  Renderer(const Renderer &rhs) = delete;
  Renderer(Renderer &&rhs) = delete;
  Renderer &operator=(const Renderer &rhs) = delete;
  Renderer &operator=(Renderer &&rhs) = delete;

  /**
   * @brief Set graph builder
   *
   * @param builderFn Builder function
   */
  void setGraphBuilder(GraphBuilderFn &&builderFn);

  /**
   * @brief Set framebuffer size
   *
   * @param size Framebuffer size
   */
  void setFramebufferSize(glm::uvec2 size);

  /**
   * @brief Rebuild render graph if settings have changed
   */
  void rebuildIfSettingsChanged();

  /**
   * @brief Execute main graph
   *
   * @param commandList Render command list
   * @param frameIndex Frame index
   */
  void execute(rhi::RenderCommandList &commandList, uint32_t frameIndex);

  /**
   * @brief Get final texture
   *
   * @return Final texture
   */
  inline rhi::TextureHandle getFinalTexture() const { return mFinalTexture; }

  /**
   * @brief Get scene texture
   *
   * @return Scene texture
   */
  inline rhi::TextureHandle getSceneTexture() const { return mSceneTexture; }

private:
  RenderStorage &mRenderStorage;

  bool mOptionsChanged = true;
  RendererOptions mOptions{};

  RenderGraph mGraph;
  GraphBuilderFn mBuilderFn{};

  rhi::TextureHandle mFinalTexture{0};
  rhi::TextureHandle mSceneTexture{0};
};

} // namespace quoll
