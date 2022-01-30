#pragma once

#include "RenderCommandList.h"
#include "liquid/entity/EntityContext.h"
#include "Pipeline.h"

namespace liquid {

class SceneRenderer {
public:
  SceneRenderer(EntityContext &entityContext, bool bindMaterialData);

  void render(RenderCommandList &commandList,
              const SharedPtr<Pipeline> &pipeline);

private:
  EntityContext &entityContext;
  bool bindMaterialData;
};

} // namespace liquid
