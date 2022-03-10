#pragma once

#include "liquid/rhi/RenderCommandList.h"
#include "liquid/entity/EntityContext.h"
#include "Pipeline.h"

namespace liquid {

class SceneRenderer {
public:
  SceneRenderer(EntityContext &entityContext, bool bindMaterialData);

  void render(RenderCommandList &commandList,
              const SharedPtr<Pipeline> &pipeline);

  void renderSkinned(RenderCommandList &commandList,
                     const SharedPtr<Pipeline> &pipeline,
                     uint32_t desciptorSet);

private:
  EntityContext &entityContext;
  bool bindMaterialData;
};

} // namespace liquid
