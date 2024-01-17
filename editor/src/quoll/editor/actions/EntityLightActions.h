#pragma once

#include "quoll/scene/CascadedShadowMap.h"
#include "quoll/scene/DirectionalLight.h"
#include "Action.h"

namespace quoll::editor {

class EntityDeleteDirectionalLight : public Action {
public:
  EntityDeleteDirectionalLight(Entity entity);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  DirectionalLight mOldDirectionalLight;
  std::optional<CascadedShadowMap> mOldCascadedShadowMap;
};

} // namespace quoll::editor
