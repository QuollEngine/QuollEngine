#include "liquid/core/Base.h"
#include "liquid/imgui/Imgui.h"

#include "liquidator/actions/SetActiveTransformActions.h"
#include "liquidator/actions/SimulationModeActions.h"
#include "liquidator/actions/CreateEmptyEntityAtViewAction.h"
#include "liquidator/actions/ExportAsGameAction.h"

#include "UIRoot.h"

namespace liquid::editor {

UIRoot::UIRoot(ActionExecutor &actionExecutor, AssetLoader &assetLoader)
    : mActionExecutor(actionExecutor), mAssetBrowser(assetLoader) {

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
  mToolbar.add(new SetActiveTransformAction(TransformOperation::Move), "Move",
               fa::Arrows, ToolbarItemType::Toggleable);
  mToolbar.add(new SetActiveTransformAction(TransformOperation::Rotate),
               "Rotate", fa::Rotate, ToolbarItemType::Toggleable);
  mToolbar.add(new SetActiveTransformAction(TransformOperation::Scale), "Scale",
               fa::ExpandAlt, ToolbarItemType::Toggleable);
}

void UIRoot::render(WorkspaceState &state, EditorManager &editorManager,
                    AssetManager &assetManager) {
  mMainMenu.render(mActionExecutor);
  mToolbar.render(state, mActionExecutor);
  mLayout.setup();

  mSceneHierarchyPanel.render(state, mActionExecutor);
  mEntityPanel.render(state, mActionExecutor, state.selectedEntity);

  EnvironmentPanel::render(state, mActionExecutor);

  mEditorCameraPanel.render(state, mActionExecutor);
  mAssetBrowser.render(assetManager, mIconRegistry, state, mActionExecutor);
}

} // namespace liquid::editor
