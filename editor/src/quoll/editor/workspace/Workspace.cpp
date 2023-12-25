#include "quoll/core/Base.h"
#include "quoll/imgui/Imgui.h"

#include "quoll/editor/core/CameraLookAt.h"
#include "quoll/editor/editor-scene/EditorCamera.h"

#include "Workspace.h"
#include "WorkspaceLayoutRenderer.h"
#include "WorkspaceIO.h"
#include "quoll/editor/asset/SceneWriter.h"

#include "quoll/editor/actions/TypedActionCreator.h"
#include "quoll/editor/actions/HistoryActions.h"
#include "quoll/editor/actions/SpawnEntityActions.h"

namespace quoll::editor {

Workspace::Workspace(Project project, AssetManager &assetManager,
                     SceneAssetHandle scene, Path scenePath, Renderer &renderer,
                     SceneRenderer &sceneRenderer,
                     EditorRenderer &editorRenderer,
                     MousePickingGraph &mousePickingGraph,
                     EditorSimulator &editorSimulator)
    : mAssetManager(assetManager), mState{project},
      mActionExecutor(mState, mAssetManager.getAssetRegistry()),
      mSceneAssetHandle(scene),
      mSceneWriter(mState.scene, mAssetManager.getAssetRegistry()),
      mSceneIO(mAssetManager.getAssetRegistry(), mState.scene),
      mRenderer(renderer), mSceneRenderer(sceneRenderer),
      mEditorRenderer(editorRenderer), mMousePickingGraph(mousePickingGraph),
      mEditorSimulator(editorSimulator), mUIRoot(assetManager) {
  mSceneIO.loadScene(scene);

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

Workspace::~Workspace() {
  ImGuiIO &io = ImGui::GetIO();

  ImGui::SaveIniSettingsToDisk(io.IniFilename);
  io.IniFilename = nullptr;
  WorkspaceIO::saveWorkspaceState(mState, mState.project.settingsPath /
                                              "default.state");
}

void Workspace::renderLayout() {
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

void Workspace::update(f32 dt) {
  mActionExecutor.process();
  mEditorSimulator.update(dt, mState);
}

void Workspace::render() {
  renderLayout();

  auto context = getContext();
  mUIRoot.render(context);
  mMouseClicked = mUIRoot.renderSceneView(context, mRenderer.getSceneTexture(),
                                          mEditorSimulator);

  auto &scene = mState.mode == WorkspaceMode::Edit ? mState.scene
                                                   : mState.simulationScene;

  mEditorSimulator.render(scene.entityDatabase);
}

void Workspace::processShortcuts(int key, int mods) {
  mShortcutsManager.process(key, mods, mActionExecutor);
}

void Workspace::updateFrameData(rhi::RenderCommandList &commandList,
                                u32 frameIndex) {
  auto &scene = mState.mode == WorkspaceMode::Edit ? mState.scene
                                                   : mState.simulationScene;

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

WorkspaceMatchParams Workspace::getMatchParams() const {
  return {.type = "SceneEditor",
          .asset = static_cast<u32>(mSceneAssetHandle),
          .assetType = AssetType::Scene};
}

} // namespace quoll::editor
