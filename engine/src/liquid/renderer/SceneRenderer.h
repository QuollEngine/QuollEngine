#pragma once

#include "liquid/entity/EntityContext.h"
#include "liquid/rhi/RenderCommandList.h"
#include "RenderStorage.h"

namespace liquid {

class SceneRenderer {
public:
  SceneRenderer(EntityContext &entityContext);

  /**
   * @deprecated This function is used in Pong3D demo
   *             because the demo uses custom render
   *             passes
   */
  void render(rhi::RenderCommandList &commandList, rhi::PipelineHandle pipeline,
              bool bindMaterialData = false);

  void render(rhi::RenderCommandList &commandList, rhi::PipelineHandle pipeline,
              RenderStorage &renderStorage, bool bindMaterialData = false);

  void renderSkinned(rhi::RenderCommandList &commandList,
                     rhi::PipelineHandle pipeline, RenderStorage &renderStorage,
                     bool bindMaterialData = true);

private:
  EntityContext &mEntityContext;
};

} // namespace liquid
