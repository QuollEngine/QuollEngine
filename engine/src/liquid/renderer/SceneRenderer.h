#pragma once

#include "liquid/entity/EntityContext.h"
#include "liquid/rhi/RenderCommandList.h"

namespace liquid {

class SceneRenderer {
public:
  SceneRenderer(EntityContext &entityContext, bool bindMaterialData = false);

  void render(rhi::RenderCommandList &commandList, rhi::PipelineHandle pipeline,
              bool bindMaterialData = false);

  void renderSkinned(rhi::RenderCommandList &commandList,
                     rhi::PipelineHandle pipeline, uint32_t desciptorSet,
                     bool bindMaterialData = true);

private:
  EntityContext &mEntityContext;
  bool mBindMaterialData;
};

} // namespace liquid
