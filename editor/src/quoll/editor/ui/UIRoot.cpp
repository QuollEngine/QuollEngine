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

#include "UIRoot.h"
#include "ImGuizmo.h"

namespace quoll::editor {

UIRoot::UIRoot(AssetManager &assetManager) {
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

void UIRoot::render(WorkspaceContext &context) {
  mMainMenu.render(context.actionExecutor);
  mToolbar.render(context.state, context.assetManager.getAssetRegistry(),
                  context.actionExecutor);

  mSceneHierarchyPanel.render(context.state, context.actionExecutor);
  mInspector.render(context.state, context.assetManager.getAssetRegistry(),
                    context.actionExecutor);

  mEditorCameraPanel.render(context.state, context.actionExecutor);
  mAssetBrowser.render(context);
}

bool UIRoot::renderSceneView(WorkspaceContext &context,
                             rhi::TextureHandle sceneTexture,
                             EditorSimulator &editorSimulator) {
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

    if (context.state.selectedEntity != Entity::Null) {
      isItemClicked &=
          !mSceneGizmos.render(context.state, context.actionExecutor);
    }
    return isItemClicked;
  }

  return false;
}

} // namespace quoll::editor
