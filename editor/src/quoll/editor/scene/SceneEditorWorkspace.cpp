#include "quoll/core/Base.h"
#include "quoll/imgui/Imgui.h"

#include "quoll/editor/core/CameraLookAt.h"
#include "quoll/editor/workspace/WorkspaceLayoutRenderer.h"
#include "quoll/editor/workspace/WorkspaceIO.h"
#include "quoll/editor/actions/TypedActionCreator.h"
#include "quoll/editor/actions/HistoryActions.h"
#include "quoll/editor/actions/SpawnEntityActions.h"

#include "core/EditorCamera.h"
#include "SceneEditorWorkspace.h"

namespace quoll::editor {

SceneEditorWorkspace::SceneEditorWorkspace(
    Project project, AssetManager &assetManager, SceneAssetHandle scene,
    Path scenePath, Renderer &renderer, SceneRenderer &sceneRenderer,
    EditorRenderer &editorRenderer, MousePickingGraph &mousePickingGraph,
    SceneSimulator &editorSimulator, WorkspaceManager &workspaceManager)
    : mAssetManager(assetManager), mState{project},
      mActionExecutor(mState, mAssetManager.getAssetRegistry()),
      mSceneAssetHandle(scene),
      mSceneWriter(mState.scene, mAssetManager.getAssetRegistry()),
      mSceneIO(mAssetManager.getAssetRegistry(), mState.scene),
      mRenderer(renderer), mSceneRenderer(sceneRenderer),
      mEditorRenderer(editorRenderer), mMousePickingGraph(mousePickingGraph),
      mEditorSimulator(editorSimulator), mWorkspaceManager(workspaceManager) {
  mSceneIO.loadScene(scene);

  mSceneWriter.open(scenePath);
  mActionExecutor.setAssetSyncer(&mSceneWriter);

  mLayoutPath = (project.settingsPath / "layout.ini").string();
  ImGuiIO &io = ImGui::GetIO();
  io.IniFilename = mLayoutPath.c_str();
  ImGui::LoadIniSettingsFromDisk(io.IniFilename);

  auto statePath = project.settingsPath / "default.state";

  mState.scene.entityDatabase.reg<CameraLookAt>();
  mState.camera =
      EditorCamera::createDefaultCamera(mState.scene.entityDatabase);
  mState.activeCamera = mState.camera;

  WorkspaceIO::loadWorkspaceState(mState, statePath);

  mRequiresDockspaceInit = !ImGui::GetCurrentContext()->SettingsLoaded;

  mShortcutsManager.add(Shortcut().control().key('N'),
                        TypedActionCreator::create<SpawnEmptyEntityAtView>());
  mShortcutsManager.add(Shortcut().control().shift().key('Z'),
                        TypedActionCreator::create<Redo>(mActionExecutor));
  mShortcutsManager.add(Shortcut().control().key('Z'),
                        TypedActionCreator::create<Undo>(mActionExecutor));
}

SceneEditorWorkspace::~SceneEditorWorkspace() {
  ImGuiIO &io = ImGui::GetIO();

  ImGui::SaveIniSettingsToDisk(io.IniFilename);
  io.IniFilename = nullptr;
  WorkspaceIO::saveWorkspaceState(mState, mState.project.settingsPath /
                                              "default.state");
}

void SceneEditorWorkspace::renderLayout() {
  if (WorkspaceLayoutRenderer::begin()) {
    if (mRequiresDockspaceInit) {
      WorkspaceLayoutRenderer::reset();
    } else {
      WorkspaceLayoutRenderer::resize();
    }
    mRequiresDockspaceInit = false;
  }
  WorkspaceLayoutRenderer::end();
}

void SceneEditorWorkspace::prepare() {}

void SceneEditorWorkspace::update(f32 dt) {
  mActionExecutor.process();
  mEditorSimulator.updateEditor(dt, mState);
}

void SceneEditorWorkspace::render() {
  renderLayout();

  mUIRoot.render(mState, mAssetManager, mActionExecutor, mSceneAssetHandle,
                 mRenderer, mSceneRenderer, mEditorRenderer, mMousePickingGraph,
                 mEditorSimulator, mWorkspaceManager);
  mMouseClicked =
      mUIRoot.renderSceneView(mState, mAssetManager, mActionExecutor,
                              mRenderer.getSceneTexture(), mEditorSimulator);

  auto &scene = mState.scene;

  mEditorSimulator.render(scene.entityDatabase);
}

void SceneEditorWorkspace::processShortcuts(int key, int mods) {
  mShortcutsManager.process(key, mods, mActionExecutor);
}

void SceneEditorWorkspace::updateFrameData(rhi::RenderCommandList &commandList,
                                           u32 frameIndex) {
  auto &scene = mState.scene;

  mSceneRenderer.updateFrameData(scene.entityDatabase, mState.activeCamera,
                                 frameIndex);
  mEditorRenderer.updateFrameData(scene.entityDatabase, mState.activeCamera,
                                  mState, mAssetManager.getAssetRegistry(),
                                  frameIndex);

  if (mMousePickingGraph.isSelectionPerformedInFrame(frameIndex)) {
    auto entity = mMousePickingGraph.getSelectedEntity();
    mState.selectedEntity = entity;
  }

  if (mMouseClicked) {
    auto mousePos = mEditorSimulator.getWindow().getCurrentMousePosition();

    if (mEditorSimulator.getEditorCamera().isWithinViewport(mousePos)) {
      auto scaledMousePos =
          mEditorSimulator.getEditorCamera().scaleToViewport(mousePos);
      mMousePickingGraph.execute(commandList, scaledMousePos, frameIndex);
    }
    mMouseClicked = false;
  }
}

WorkspaceMatchParams SceneEditorWorkspace::getMatchParams() const {
  return {.type = "SceneEditor",
          .asset = static_cast<u32>(mSceneAssetHandle),
          .assetType = AssetType::Scene};
}

void SceneEditorWorkspace::reload() { mUIRoot.getAssetBrowser().reload(); }

} // namespace quoll::editor
