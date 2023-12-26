#include "quoll/core/Base.h"
#include "quoll/imgui/Imgui.h"

#include "quoll/editor/actions/TransformOperationActions.h"
#include "quoll/editor/actions/SimulationModeActions.h"
#include "quoll/editor/actions/SpawnEntityActions.h"
#include "quoll/editor/actions/ProjectActions.h"
#include "quoll/editor/actions/DeleteEntityAction.h"
#include "quoll/editor/actions/TypedActionCreator.h"
#include "quoll/editor/actions/HistoryActions.h"
#include "quoll/editor/ui/FontAwesome.h"
#include "quoll/editor/ui/Shortcut.h"

#include "SceneEditorUI.h"

#include "ImGuizmo.h"

namespace quoll::editor {

SceneEditorUI::SceneEditorUI(AssetManager &assetManager) {
  mMainMenu.begin("Project")
      .add("Export as game",
           TypedActionCreator::create<ExportAsGame>(assetManager))
      .end()
      .begin("Objects")
      .add("Create empty object",
           TypedActionCreator::create<SpawnEmptyEntityAtView>(),
           Shortcut().control().key('N'))
      .end();

  mToolbar.add(TypedActionCreator::create<StartSimulationMode>(), "Play",
               fa::Play, ToolbarItemType::HideWhenInactive);
  mToolbar.add(TypedActionCreator::create<StopSimulationMode>(), "Stop",
               fa::Stop, ToolbarItemType::HideWhenInactive);
  mToolbar.add(
      TypedActionCreator::create<SetActiveTransform>(TransformOperation::Move),
      "Move", fa::Arrows, ToolbarItemType::Toggleable);
  mToolbar.add(TypedActionCreator ::create<SetActiveTransform>(
                   TransformOperation::Rotate),
               "Rotate", fa::Rotate, ToolbarItemType::Toggleable);
  mToolbar.add(
      TypedActionCreator::create<SetActiveTransform>(TransformOperation::Scale),
      "Scale", fa::ExpandAlt, ToolbarItemType::Toggleable);
}

void SceneEditorUI::render(WorkspaceState &state, AssetManager &assetManager,
                           ActionExecutor &actionExecutor) {
  mMainMenu.render(actionExecutor);
  mToolbar.render(state, assetManager.getAssetRegistry(), actionExecutor);

  mSceneHierarchyPanel.render(state, actionExecutor);
  mInspector.render(state, assetManager.getAssetRegistry(), actionExecutor);

  mEditorCameraPanel.render(state, actionExecutor);
  mAssetBrowser.render(state, assetManager, actionExecutor);
}

bool SceneEditorUI::renderSceneView(WorkspaceState &state,
                                    ActionExecutor &actionExecutor,
                                    rhi::TextureHandle sceneTexture,
                                    SceneSimulator &editorSimulator) {
  if (auto _ = SceneView(sceneTexture)) {
    const auto &pos = ImGui::GetItemRectMin();
    const auto &size = ImGui::GetItemRectSize();

    auto &aspectRatioUpdater = editorSimulator.getCameraAspectRatioUpdater();
    auto &uiCanvasUpdater = editorSimulator.getUICanvasUpdater();
    auto &editorCamera = editorSimulator.getEditorCamera();

    editorCamera.setViewport(pos.x, pos.y, size.x, size.y,
                             ImGui::IsItemHovered());
    uiCanvasUpdater.setViewport(pos.x, pos.y, size.x, size.y);

    aspectRatioUpdater.setViewportSize(
        {static_cast<u32>(size.x), static_cast<u32>(size.y)});

    bool isItemClicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);

    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(pos.x, pos.y, size.x, size.y);

    if (state.selectedEntity != Entity::Null) {
      isItemClicked &= !mSceneGizmos.render(state, actionExecutor);
    }
    return isItemClicked;
  }

  return false;
}

} // namespace quoll::editor
