#include "liquid/core/Base.h"
#include "liquid/imgui/Imgui.h"

#include "liquidator/actions/TransformOperationActions.h"
#include "liquidator/actions/SimulationModeActions.h"
#include "liquidator/actions/SpawnEntityActions.h"
#include "liquidator/actions/ProjectActions.h"
#include "liquidator/actions/DeleteEntityAction.h"
#include "liquidator/actions/TypedActionCreator.h"
#include "liquidator/actions/HistoryActions.h"

#include "UIRoot.h"
#include "ImGuizmo.h"

namespace liquid::editor {

UIRoot::UIRoot() {
  mMainMenu.begin("Project")
      .add("Export as game", TypedActionCreator::create<ExportAsGame>())
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
                             EditorCamera &editorCamera) {
  if (auto _ = SceneView(sceneTexture)) {
    const auto &pos = ImGui::GetItemRectMin();
    const auto &size = ImGui::GetItemRectSize();

    editorCamera.setViewport(pos.x, pos.y, size.x, size.y,
                             ImGui::IsItemHovered());

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

} // namespace liquid::editor
