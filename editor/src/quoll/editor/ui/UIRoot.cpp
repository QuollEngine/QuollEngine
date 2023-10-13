#include "quoll/core/Base.h"
#include "quoll/imgui/Imgui.h"

#include "quoll/editor/actions/TransformOperationActions.h"
#include "quoll/editor/actions/SimulationModeActions.h"
#include "quoll/editor/actions/SpawnEntityActions.h"
#include "quoll/editor/actions/ProjectActions.h"
#include "quoll/editor/actions/DeleteEntityAction.h"
#include "quoll/editor/actions/TypedActionCreator.h"
#include "quoll/editor/actions/HistoryActions.h"

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
                             EditorCamera &editorCamera,
                             CameraAspectRatioUpdater &aspectRatioUpdater,
                             UICanvasUpdater &uiCanvasUpdater) {
  if (auto _ = SceneView(sceneTexture)) {
    const auto &pos = ImGui::GetItemRectMin();
    const auto &size = ImGui::GetItemRectSize();

    editorCamera.setViewport(pos.x, pos.y, size.x, size.y,
                             ImGui::IsItemHovered());
    uiCanvasUpdater.setViewport(pos.x, pos.y, size.x, size.y);

    aspectRatioUpdater.setViewportSize(
        {static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y)});

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

void UIRoot::processShortcuts(WorkspaceContext &context,
                              EventSystem &eventSystem) {
  mShortcutsManager.add(Shortcut().control().key('N'),
                        TypedActionCreator::create<SpawnEmptyEntityAtView>());
  mShortcutsManager.add(
      Shortcut().control().shift().key('Z'),
      TypedActionCreator::create<Redo>(context.actionExecutor));
  mShortcutsManager.add(
      Shortcut().control().key('Z'),
      TypedActionCreator::create<Undo>(context.actionExecutor));

  eventSystem.observe(
      KeyboardEvent::Pressed, [this, &context](const auto &data) {
        mShortcutsManager.process(data.key, data.mods, context.actionExecutor);
      });
}

} // namespace quoll::editor
