#include "quoll/core/Base.h"
#include "quoll/imgui/Imgui.h"
#include "quoll/editor/actions/DeleteEntityAction.h"
#include "quoll/editor/actions/HistoryActions.h"
#include "quoll/editor/actions/ProjectActions.h"
#include "quoll/editor/actions/SpawnEntityActions.h"
#include "quoll/editor/actions/TransformOperationActions.h"
#include "quoll/editor/actions/TypedActionCreator.h"
#include "quoll/editor/actions/WorkspaceActions.h"
#include "quoll/editor/ui/FontAwesome.h"
#include "quoll/editor/ui/MainMenuBar.h"
#include "quoll/editor/ui/Menu.h"
#include "quoll/editor/ui/MenuBar.h"
#include "quoll/editor/ui/Shortcut.h"
#include "quoll/editor/ui/Toolbar.h"
#include "quoll/editor/ui/Widgets.h"
#include "../SceneSimulatorWorkspace.h"
#include "ImGuizmo.h"
#include "SceneEditorUI.h"

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
                          ActionExecutor &actionExecutor,
                          AssetHandle<SceneAsset> sceneHandle,
                          Renderer &renderer, SceneRenderer &sceneRenderer,
                          EditorRenderer &editorRenderer,
                          MousePickingGraph &mousePickingGraph,
                          MainEngineModules &engineModules,
                          EditorCamera &editorCamera,
                          WorkspaceManager &workspaceManager) {
  if (auto toolbar = Toolbar()) {
    if (toolbar.item("Play the scene", fa::Play, false)) {

      auto *workspace = new SceneSimulatorWorkspace(
          state.project, assetManager, sceneHandle, state.scene, renderer,
          sceneRenderer, editorRenderer, mousePickingGraph, engineModules,
          editorCamera);

      actionExecutor.execute<AddWorkspace>(workspace, workspaceManager);
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
                           ActionExecutor &actionExecutor,
                           AssetHandle<SceneAsset> sceneHandle,
                           Renderer &renderer, SceneRenderer &sceneRenderer,
                           EditorRenderer &editorRenderer,
                           MousePickingGraph &mousePickingGraph,
                           MainEngineModules &engineModules,
                           EditorCamera &editorCamera,
                           WorkspaceManager &workspaceManager) {
  if (auto _ = MainMenuBar()) {
    if (auto projects = Menu("Projects")) {
      if (projects.item("Export as game")) {
        actionExecutor.execute<ExportAsGame>(assetManager);
      }
    }
  }

  renderToolbar(state, assetManager, actionExecutor, sceneHandle, renderer,
                sceneRenderer, editorRenderer, mousePickingGraph, engineModules,
                editorCamera, workspaceManager);

  mSceneHierarchyPanel.render(state, actionExecutor);
  mInspector.render(state, assetManager.getCache(), actionExecutor);

  mAssetBrowser.render(state, assetManager, actionExecutor);
}

bool SceneEditorUI::renderSceneView(WorkspaceState &state,
                                    AssetManager &assetManager,
                                    ActionExecutor &actionExecutor,
                                    rhi::TextureHandle sceneTexture,
                                    MainEngineModules &engineModules,
                                    EditorCamera &editorCamera) {
  mEditorCameraPanel.render(state, actionExecutor);

  if (auto _ = SceneView(sceneTexture)) {
    const auto &pos = ImGui::GetItemRectMin();
    const auto &size = ImGui::GetItemRectSize();

    auto &aspectRatioUpdater = engineModules.getCameraAspectRatioUpdater();
    auto &uiCanvasUpdater = engineModules.getUICanvasUpdater();

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
