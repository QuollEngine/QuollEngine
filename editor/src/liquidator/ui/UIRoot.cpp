#include "liquid/core/Base.h"
#include "liquid/imgui/Imgui.h"

#include "liquidator/actions/SetActiveTransformActions.h"
#include "liquidator/actions/SimulationModeActions.h"
#include "liquidator/actions/CreateEmptyEntityAtViewAction.h"
#include "liquidator/actions/ExportAsGameAction.h"

#include "UIRoot.h"

namespace liquid::editor {

UIRoot::UIRoot(ActionExecutor &actionExecutor, EntityManager &entityManager,
               AssetLoader &assetLoader)
    : mActionExecutor(actionExecutor), mAssetBrowser(assetLoader),
      mEntityPanel(entityManager) {

  mMainMenu.begin("Project")
      .add("Export as game", new ExportAsGameAction)
      .end()
      .begin("Objects")
      .add("Create empty object", new CreateEmptyEntityAtViewAction)
      .end();

  mToolbar.add(new StartSimulationModeAction, "Play", fa::Play,
               ToolbarItemType::HideWhenInactive);
  mToolbar.add(new StopSimulationModeAction, "Stop", fa::Stop,
               ToolbarItemType::HideWhenInactive);
  mToolbar.add(new SetActiveTransformToMoveAction, "Move", fa::Arrows,
               ToolbarItemType::Toggleable);
  mToolbar.add(new SetActiveTransformToRotateAction, "Rotate", fa::Rotate,
               ToolbarItemType::Toggleable);
  mToolbar.add(new SetActiveTransformToScaleAction, "Scale", fa::ExpandAlt,
               ToolbarItemType::Toggleable);
}

void UIRoot::render(WorkspaceState &state, EditorManager &editorManager,
                    Renderer &renderer, AssetManager &assetManager,
                    PhysicsSystem &physicsSystem,
                    EntityManager &entityManager) {
  mMainMenu.render(mActionExecutor);
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
