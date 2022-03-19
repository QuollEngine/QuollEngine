#pragma once

#include "liquid/entity/EntityContext.h"
#include "liquid/rhi/RenderCommandList.h"

namespace liquid {

class SceneRenderer {
public:
  SceneRenderer(EntityContext &entityContext, bool bindMaterialData);

  void render(rhi::RenderCommandList &commandList,
              rhi::PipelineHandle pipeline);

  void renderSkinned(rhi::RenderCommandList &commandList,
                     rhi::PipelineHandle pipeline, uint32_t desciptorSet);

private:
  EntityContext &mEntityContext;
  bool mBindMaterialData;
};

} // namespace liquid
