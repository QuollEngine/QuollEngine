#pragma once

#include "liquid/entity/EntityContext.h"
#include "liquid/rhi/RenderCommandList.h"

namespace liquid {

class SceneRenderer {
public:
  SceneRenderer(EntityContext &entityContext, bool bindMaterialData);

  void render(RenderCommandList &commandList, PipelineHandle pipeline);

  void renderSkinned(RenderCommandList &commandList, PipelineHandle pipeline,
                     uint32_t desciptorSet);

private:
  EntityContext &entityContext;
  bool bindMaterialData;
};

} // namespace liquid
