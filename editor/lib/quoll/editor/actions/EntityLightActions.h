#pragma once

#include "quoll/scene/CascadedShadowMap.h"
#include "quoll/scene/DirectionalLight.h"
#include "Action.h"

namespace quoll::editor {

class EntityDeleteDirectionalLight : public Action {
public:
  EntityDeleteDirectionalLight(Entity entity);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  Entity mEntity;
  DirectionalLight mOldDirectionalLight;
  std::optional<CascadedShadowMap> mOldCascadedShadowMap;
};

} // namespace quoll::editor
