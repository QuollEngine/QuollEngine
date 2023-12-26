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
#include "quoll/editor/ui/Toolbar.h"
#include "quoll/editor/ui/Menu.h"
#include "quoll/editor/ui/MenuBar.h"
#include "quoll/editor/ui/MainMenuBar.h"
#include "quoll/editor/ui/Widgets.h"

#include "SceneEditorUI.h"

#include "ImGuizmo.h"

namespace quoll::editor {

static void renderMainMenu(WorkspaceState &state, AssetManager &assetManager,
                           ActionExecutor &actionExecutor) {
  if (auto _ = MenuBar()) {
    if (auto objects = Menu("Objects")) {
      if (objects.item("Create empty object", Shortcut().control().key('N'))) {
        actionExecutor.execute<SpawnEmptyEntityAtView>();
      }
    }
  }
}

static void renderToolbar(WorkspaceState &state, AssetManager &assetManager,
                          ActionExecutor &actionExecutor, Renderer &renderer,
                          SceneRenderer &sceneRenderer,
                          EditorRenderer &editorRenderer,
                          MousePickingGraph &mousePickingGraph,
                          SceneSimulator &editorSimulator,
                          WorkspaceManager &workspaceManager) {
  if (auto toolbar = Toolbar()) {
    if (state.mode == WorkspaceMode::Edit &&
        toolbar.item("Start simulation mode", fa::Play, false)) {
      actionExecutor.execute<StartSimulationMode>();
    }

    if (state.mode == WorkspaceMode::Simulation &&
        toolbar.item("Stop simulation mode", fa::Stop, false)) {
      actionExecutor.execute<StopSimulationMode>();
    }

    if (toolbar.item("Move", fa::Arrows,
                     state.activeTransform == TransformOperation::Move)) {
      actionExecutor.execute<SetActiveTransform>(TransformOperation::Move);
    }

    if (toolbar.item("Rotate", fa::Rotate,
                     state.activeTransform == TransformOperation::Rotate)) {
      actionExecutor.execute<SetActiveTransform>(TransformOperation::Rotate);
    }

    if (toolbar.item("Scale", fa::ExpandAlt,
                     state.activeTransform == TransformOperation::Scale)) {
      actionExecutor.execute<SetActiveTransform>(TransformOperation::Scale);
    }
  }
}

void SceneEditorUI::render(WorkspaceState &state, AssetManager &assetManager,
                           ActionExecutor &actionExecutor, Renderer &renderer,
                           SceneRenderer &sceneRenderer,
                           EditorRenderer &editorRenderer,
                           MousePickingGraph &mousePickingGraph,
                           SceneSimulator &editorSimulator,
                           WorkspaceManager &workspaceManager) {
  if (auto _ = MainMenuBar()) {
    if (auto projects = Menu("Projects")) {
      if (projects.item("Export as game")) {
        actionExecutor.execute<ExportAsGame>(assetManager);
      }
    }
  }

  renderToolbar(state, assetManager, actionExecutor, renderer, sceneRenderer,
                editorRenderer, mousePickingGraph, editorSimulator,
                workspaceManager);

  mSceneHierarchyPanel.render(state, actionExecutor);
  mInspector.render(state, assetManager.getAssetRegistry(), actionExecutor);

  mAssetBrowser.render(state, assetManager, actionExecutor);
}

bool SceneEditorUI::renderSceneView(WorkspaceState &state,
                                    AssetManager &assetManager,
                                    ActionExecutor &actionExecutor,
                                    rhi::TextureHandle sceneTexture,
                                    SceneSimulator &editorSimulator) {
  mEditorCameraPanel.render(state, actionExecutor);

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

    renderMainMenu(state, assetManager, actionExecutor);
    mEditorCameraPanel.renderMenu();

    return isItemClicked;
  }

  return false;
}

} // namespace quoll::editor
