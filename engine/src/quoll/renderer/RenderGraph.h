#pragma once

#include "quoll/rhi/RenderDevice.h"

#include "RenderStorage.h"
#include "RenderGraphRegistry.h"
#include "RenderGraphResource.h"
#include "RenderGraphPass.h"

namespace quoll {

enum class GraphDirty { None, PassChanges, SizeUpdate };

/**
 * @brief Render graph
 */
class RenderGraph {
  using RGTexture = RenderGraphResource<rhi::TextureHandle>;
  using RGTextureCreator = std::function<rhi::TextureDescription(u32, u32)>;
  template <class THandle>
  using RGBuildCallback = std::function<void(THandle, RenderStorage &)>;

  using RGTextureBuildCallback = RGBuildCallback<rhi::TextureHandle>;

  enum class RGResourceType { Texture, Buffer };

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
   * @brief Create fixed texture
   *
   * @param description Texture description
   * @return Render graph texture
   */
  RGTexture create(const rhi::TextureDescription &description);

  /**
   * @brief Create texture view
   *
   * @param texture Render graph texture
   * @param baseMipLevel Base mip level
   * @param mipLevelCount Mip level count
   * @param baseLayer Base layer
   * @param layerCount Layer count
   * @return Render graph texture
   */
  RGTexture createView(RGTexture texture, u32 baseMipLevel = 0,
                       u32 mipLevelCount = 1, u32 baseLayer = 0,
                       u32 layerCount = 1);

  /**
   * @brief Import existing texture to render graph
   *
   * @param handle Texture handle
   * @return Render graph texture
   */
  RGTexture import(rhi::TextureHandle handle);

  /**
   * @brief Execute render graph
   *
   * @param commandList Command list
   * @param frameIndex Frame index
   */
  void execute(rhi::RenderCommandList &commandList, u32 frameIndex);

  /**
   * @brief Build render graph
   *
   * @param storage Render storage
   */
  void build(RenderStorage &storage);

  /**
   * @brief Clear render graph
   *
   * @param storage Render storage
   */
  void destroy(RenderStorage &storage);

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
   * @brief Get name
   *
   * @return Render graph name
   */
  inline const String &getName() const { return mName; }

private:
  /**
   * @brief Build render graph resources
   *
   * @param storage Render storage
   */
  void buildResources(RenderStorage &storage);

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
  RenderGraphRegistry mRegistry;
  String mName;

  std::vector<RenderGraphPass> mPasses;
  std::vector<RenderGraphPass> mCompiledPasses;
};

} // namespace quoll
