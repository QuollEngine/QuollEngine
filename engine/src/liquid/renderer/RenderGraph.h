#pragma once

#include "liquid/rhi/RenderDevice.h"

#include "RenderGraphPass.h"
#include "RenderStorage.h"

namespace liquid {

enum class GraphDirty { None, PassChanges, SizeUpdate };

/**
 * @brief Render graph
 */
class RenderGraph {
public:
  /**
   * @brief Initialize render graph with name
   *
   * @param name Render graph name
   */
  RenderGraph(StringView name);

  /**
   * @brief Add graphics pass
   *
   * @param name Pass name
   * @return Render graph pass
   */
  RenderGraphPass &addGraphicsPass(StringView name);

  /**
   * @brief Add compute pass
   *
   * @param name Pass name
   * @return Render graph pass
   */
  RenderGraphPass &addComputePass(StringView name);

  /**
   * @brief Execute render graph
   *
   * @param commandList Command list
   * @param frameIndex Frame index
   */
  void execute(rhi::RenderCommandList &commandList, uint32_t frameIndex);

  /**
   * @brief Build render graph
   *
   * @param storage Render storage
   */
  void build(RenderStorage &storage);

  /**
   * @brief Get passes
   *
   * @return Render graph passes
   */
  inline std::vector<RenderGraphPass> &getPasses() { return mPasses; }

  /**
   * @brief Get compiled passes
   *
   * @return Compiled render graph passes
   */
  inline std::vector<RenderGraphPass> &getCompiledPasses() {
    return mCompiledPasses;
  }

  /**
   * @brief Set framebuffer extent
   *
   * @param framebufferExtent Framebuffer extent
   */
  void setFramebufferExtent(glm::uvec2 framebufferExtent);

  /**
   * @brief Get framebuffer extent
   *
   * @return Framebuffer extent
   */
  inline const glm::uvec2 &getFramebufferExtent() const {
    return mFramebufferExtent;
  }

  /**
   * @brief Check if recreate is necessary
   *
   * @retval true Passes have changed
   * @retval false Passes have not changed
   */
  inline bool isDirty() const { return mDirty != GraphDirty::None; }

  /**
   * @brief Get name
   *
   * @return Render graph name
   */
  inline const String &getName() const { return mName; }

private:
  /**
   * @brief Compile render graph
   *
   * Topologically sorts and updates render
   * passes in place
   */
  void compile();

  /**
   * @brief Build barriers
   */
  void buildBarriers();

  /**
   * @brief Build passes resources
   *
   *
   * @param storage Render storage
   */
  void buildPasses(RenderStorage &storage);

  /**
   * @brief Build graphics pass resources
   *
   * Creates framebuffers, render passes,
   * and pipelines
   *
   * @param pass Render graph pass
   * @param storage Render storage
   */
  void buildGraphicsPass(RenderGraphPass &pass, RenderStorage &storage);

  /**
   * @brief Build compute pass resources
   *
   * Creates compute pipelines
   *
   * @param pass Render graph pass
   * @param storage Render storage
   */
  void buildComputePass(RenderGraphPass &pass, RenderStorage &storage);

private:
  std::vector<RenderGraphPass> mPasses;
  std::vector<RenderGraphPass> mCompiledPasses;

  glm::uvec2 mFramebufferExtent{};
  GraphDirty mDirty = GraphDirty::None;

  String mName;
};

} // namespace liquid
