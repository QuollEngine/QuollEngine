#include "liquid/core/Base.h"
#include "liquid/imgui/Imgui.h"

#include "liquidator/actions/SetActiveTransformActions.h"
#include "liquidator/actions/SimulationModeActions.h"

#include "UIRoot.h"

namespace liquid::editor {

UIRoot::UIRoot(ActionExecutor &actionExecutor, EntityManager &entityManager,
               AssetLoader &assetLoader)
    : mActionExecutor(actionExecutor), mAssetBrowser(assetLoader),
      mEntityPanel(entityManager) {

  mToolbar.add(StartSimulationModeAction, "Play", fa::Play,
               ToolbarItemType::HideWhenInactive);
  mToolbar.add(StopSimulationModeAction, "Stop", fa::Stop,
               ToolbarItemType::HideWhenInactive);
  mToolbar.add(SetActiveTransformToMoveAction, "Move", fa::Arrows,
               ToolbarItemType::Toggleable);
  mToolbar.add(SetActiveTransformToRotateAction, "Rotate", fa::Rotate,
               ToolbarItemType::Toggleable);
  mToolbar.add(SetActiveTransformToScaleAction, "Scale", fa::ExpandAlt,
               ToolbarItemType::Toggleable);
}

void UIRoot::render(WorkspaceState &state, EditorManager &editorManager,
                    Renderer &renderer, AssetManager &assetManager,
                    PhysicsSystem &physicsSystem,
                    EntityManager &entityManager) {
  mToolbar.render(state, mActionExecutor);
  mLayout.setup();

  mSceneHierarchyPanel.render(state, mActionExecutor);

  if (state.selectedEntity != Entity::Null) {
    mEntityPanel.render(editorManager, state.selectedEntity, renderer,
                        assetManager, physicsSystem);
  }

  EnvironmentPanel::render(editorManager, assetManager);

  mEditorCameraPanel.render(state, mActionExecutor);
  mAssetBrowser.render(assetManager, mIconRegistry, state, editorManager,
                       entityManager);
}

} // namespace liquid::editor
