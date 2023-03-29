#include "liquid/core/Base.h"
#include "liquid/imgui/Imgui.h"

#include "liquidator/actions/TransformOperationActions.h"
#include "liquidator/actions/SimulationModeActions.h"
#include "liquidator/actions/SpawnEntityActions.h"
#include "liquidator/actions/ProjectActions.h"
#include "liquidator/actions/DeleteEntityAction.h"

#include "UIRoot.h"
#include "ImGuizmo.h"

namespace liquid::editor {

UIRoot::UIRoot(ActionExecutor &actionExecutor, AssetLoader &assetLoader)
    : mActionExecutor(actionExecutor), mAssetBrowser(assetLoader) {

  mShortcutsManager.add(Shortcut().control().key('N'),
                        new SpawnEmptyEntityAtView);

  mMainMenu.begin("Project")
      .add("Export as game", new ExportAsGame)
      .end()
      .begin("Objects")
      .add("Create empty object", new SpawnEmptyEntityAtView,
           Shortcut().control().key('N'))
      .end();

  mToolbar.add(new StartSimulationMode, "Play", fa::Play,
               ToolbarItemType::HideWhenInactive);
  mToolbar.add(new StopSimulationMode, "Stop", fa::Stop,
               ToolbarItemType::HideWhenInactive);
  mToolbar.add(new SetActiveTransform(TransformOperation::Move), "Move",
               fa::Arrows, ToolbarItemType::Toggleable);
  mToolbar.add(new SetActiveTransform(TransformOperation::Rotate), "Rotate",
               fa::Rotate, ToolbarItemType::Toggleable);
  mToolbar.add(new SetActiveTransform(TransformOperation::Scale), "Scale",
               fa::ExpandAlt, ToolbarItemType::Toggleable);
}

void UIRoot::render(WorkspaceState &state, AssetManager &assetManager) {
  mMainMenu.render(mActionExecutor);
  mToolbar.render(state, mActionExecutor);
  mLayout.setup();

  mSceneHierarchyPanel.render(state, mActionExecutor);
  mEntityPanel.render(state, mActionExecutor, state.selectedEntity);

  EnvironmentPanel::render(state, mActionExecutor);

  mEditorCameraPanel.render(state, mActionExecutor);
  mAssetBrowser.render(assetManager, mIconRegistry, state, mActionExecutor);
}

bool UIRoot::renderSceneView(WorkspaceState &state,
                             rhi::TextureHandle sceneTexture,
                             EditorCamera &editorCamera) {

  if (auto _ = SceneView(sceneTexture)) {
    const auto &pos = ImGui::GetItemRectMin();
    const auto &size = ImGui::GetItemRectSize();

    editorCamera.setViewport(pos.x, pos.y, size.x, size.y,
                             ImGui::IsItemHovered());

    bool isItemClicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);

    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(pos.x, pos.y, size.x, size.y);

    if (state.selectedEntity != Entity::Null) {
      isItemClicked &= !mSceneGizmos.render(state, mActionExecutor);
    }
    return isItemClicked;
  }

  return false;
}

void UIRoot::processShortcuts(EventSystem &eventSystem) {
  eventSystem.observe(KeyboardEvent::Pressed, [this](const auto &data) {
    mShortcutsManager.process(data.key, data.mods, mActionExecutor);
  });
}

} // namespace liquid::editor
