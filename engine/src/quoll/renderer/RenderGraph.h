#pragma once

#include "quoll/rhi/RenderDevice.h"

#include "RenderStorage.h"
#include "RenderGraphRegistry.h"
#include "RenderGraphResource.h"
#include "RenderGraphPass.h"

namespace quoll {

enum class GraphDirty { None, PassChanges, SizeUpdate };

class RenderGraph {
  using RGTexture = RenderGraphResource<rhi::TextureHandle>;
  using RGTextureCreator = std::function<rhi::TextureDescription(u32, u32)>;
  template <class THandle>
  using RGBuildCallback = std::function<void(THandle, RenderStorage &)>;

  using RGTextureBuildCallback = RGBuildCallback<rhi::TextureHandle>;

  enum class RGResourceType { Texture, Buffer };

public:
  RenderGraph(StringView name);

  RenderGraphPass &addGraphicsPass(StringView name);

  RenderGraphPass &addComputePass(StringView name);

  RGTexture create(const rhi::TextureDescription &description);

  RGTexture createView(RGTexture texture, u32 baseMipLevel = 0,
                       u32 mipLevelCount = 1, u32 baseLayer = 0,
                       u32 layerCount = 1);

  RGTexture import(rhi::TextureHandle handle);

  void execute(rhi::RenderCommandList &commandList, u32 frameIndex);

  void build(RenderStorage &storage);

  void destroy(RenderStorage &storage);

  inline std::vector<RenderGraphPass> &getPasses() { return mPasses; }

  inline std::vector<RenderGraphPass> &getCompiledPasses() {
    return mCompiledPasses;
  }

  inline const String &getName() const { return mName; }

private:
  void buildResources(RenderStorage &storage);

  void compile();

  void buildBarriers();

  void buildPasses(RenderStorage &storage);

  void buildGraphicsPass(RenderGraphPass &pass, RenderStorage &storage);

  void buildComputePass(RenderGraphPass &pass, RenderStorage &storage);

private:
  RenderGraphRegistry mRegistry;
  String mName;

  std::vector<RenderGraphPass> mPasses;
  std::vector<RenderGraphPass> mCompiledPasses;
};

} // namespace quoll
