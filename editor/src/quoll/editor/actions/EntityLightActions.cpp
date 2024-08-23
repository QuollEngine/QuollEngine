#include "quoll/core/Base.h"
#include "DeleteEntityAction.h"
#include "EntityLightActions.h"

namespace quoll::editor {

EntityDeleteDirectionalLight::EntityDeleteDirectionalLight(Entity entity)
    : mEntity(entity) {}

ActionExecutorResult
EntityDeleteDirectionalLight::onExecute(WorkspaceState &state,
                                        AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mOldDirectionalLight = *mEntity.get_ref<DirectionalLight>().get();
  mEntity.remove<DirectionalLight>();

  if (mEntity.has<CascadedShadowMap>()) {
    mOldCascadedShadowMap = *mEntity.get_ref<CascadedShadowMap>().get();
    mEntity.remove<CascadedShadowMap>();
  }

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  res.addToHistory = true;
  return res;
}

ActionExecutorResult
EntityDeleteDirectionalLight::onUndo(WorkspaceState &state,
                                     AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mEntity.set(mOldDirectionalLight);

  if (mOldCascadedShadowMap) {
    mEntity.set(mOldCascadedShadowMap.value());
  }

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntityDeleteDirectionalLight::predicate(WorkspaceState &state,
                                             AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  return mEntity.has<DirectionalLight>();
}

} // namespace quoll::editor
