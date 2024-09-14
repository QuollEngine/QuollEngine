#pragma once

#include "Action.h"

namespace quoll::editor {

/**
 * @brief Delete entity components
 *
 * @tparam TComponent Main component to delete. Restored during undo.
 * @tparam TOtherComponents Other components to delete. Are not restored during
 * undo.
 */
template <typename TComponent, typename... TOtherComponents>
class EntityDeleteComponent : public Action {
public:
  EntityDeleteComponent(Entity entity) : mEntity(entity) {}

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override {
    auto &scene = state.scene;

    mOldComponent = scene.entityDatabase.get<TComponent>(mEntity);

    scene.entityDatabase.remove<TComponent>(mEntity);
    removeOtherComponents<TOtherComponents...>(scene);

    ActionExecutorResult res{};
    res.entitiesToSave.push_back(mEntity);
    res.addToHistory = true;

    return res;
  }

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override {
    auto &scene = state.scene;

    scene.entityDatabase.set(mEntity, mOldComponent);

    ActionExecutorResult res{};
    res.entitiesToSave.push_back(mEntity);

    return res;
  }

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override {
    auto &scene = state.scene;

    return scene.entityDatabase.has<TComponent>(mEntity);
  }

private:
  template <typename... TRest>
  requires(sizeof...(TRest) == 0)
  void removeOtherComponents(Scene &) {
    // Base case. Does nothing
  }

  template <typename TFirst, typename... TRest>
  void removeOtherComponents(Scene &scene) {
    if (scene.entityDatabase.has<TFirst>(mEntity)) {
      scene.entityDatabase.remove<TFirst>(mEntity);
    }

    removeOtherComponents<TRest...>(scene);
  }

private:
  Entity mEntity;
  TComponent mOldComponent;
};

} // namespace quoll::editor
