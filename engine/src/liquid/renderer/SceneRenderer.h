#pragma once

#include "liquid/entity/EntityContext.h"
#include "liquid/rhi/RenderCommandList.h"
#include "liquid/asset/AssetRegistry.h"
#include "RenderStorage.h"

namespace liquid {

class SceneRenderer {
public:
  void render(rhi::RenderCommandList &commandList, rhi::PipelineHandle pipeline,
              const RenderStorage &renderStorage, AssetRegistry &assetRegistry,
              bool bindMaterialData = false);

  void renderSkinned(rhi::RenderCommandList &commandList,
                     rhi::PipelineHandle pipeline,
                     const RenderStorage &renderStorage,
                     AssetRegistry &assetRegistry,
                     bool bindMaterialData = true);
};

} // namespace liquid
