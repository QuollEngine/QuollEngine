#include "quoll/core/Base.h"
#include "quoll/imgui/Imgui.h"
#include "quoll/editor/actions/HistoryActions.h"
#include "quoll/editor/actions/SpawnEntityActions.h"
#include "quoll/editor/actions/TypedActionCreator.h"
#include "quoll/editor/core/CameraLookAt.h"
#include "quoll/editor/workspace/WorkspaceIO.h"
#include "quoll/editor/workspace/WorkspaceLayoutRenderer.h"
#include "core/EditorCamera.h"
#include "SceneSimulatorWorkspace.h"

namespace quoll::editor {

SceneSimulatorWorkspace::SceneSimulatorWorkspace(
    Project project, AssetManager &assetManager, SceneAssetHandle scene,
    Scene &sourceScene, Renderer &renderer, SceneRenderer &sceneRenderer,
    EditorRenderer &editorRenderer, MousePickingGraph &mousePickingGraph,
    MainEngineModules &engineModules, EditorCamera &editorCamera)
    : mAssetManager(assetManager), mState{project},
      mActionExecutor(mState, mAssetManager.getAssetRegistry()),
      mSceneAssetHandle(scene),
      mSceneIO(mAssetManager.getAssetRegistry(), mState.scene),
      mRenderer(renderer), mSceneRenderer(sceneRenderer),
      mEditorRenderer(editorRenderer), mMousePickingGraph(mousePickingGraph),
      mEngineModules(engineModules), mEditorCamera(editorCamera) {

  sourceScene.entityDatabase.duplicate(mState.scene.entityDatabase);
  mState.scene.dummyCamera = sourceScene.dummyCamera;
  mState.scene.activeEnvironment = sourceScene.activeEnvironment;
  mState.scene.dummyEnvironment = sourceScene.dummyEnvironment;
  mState.mode = WorkspaceMode::Simulation;

  if (sourceScene.entityDatabase.has<Camera>(sourceScene.activeCamera)) {
    mState.scene.activeCamera = sourceScene.activeCamera;
    mState.activeCamera = sourceScene.activeCamera;
  } else {
    mState.scene.activeCamera = sourceScene.dummyCamera;
    mState.activeCamera = sourceScene.dummyCamera;
  }

  mState.camera = mState.activeCamera;

  mRequiresDockspaceInit = true;

  ImGuiIO &io = ImGui::GetIO();
  io.IniFilename = nullptr;

  mEngineModules.observeChanges(mState.scene.entityDatabase);
}

SceneSimulatorWorkspace::~SceneSimulatorWorkspace() {
  mEngineModules.cleanupObservers(mState.scene.entityDatabase);
}

void SceneSimulatorWorkspace::prepare() {
  mEngineModules.prepare(mState.scene);
}

void SceneSimulatorWorkspace::fixedUpdate(f32 dt) {
  mEngineModules.fixedUpdate(dt, mState.scene);
}

void SceneSimulatorWorkspace::update(f32 dt) {
  mActionExecutor.process();
  mEngineModules.update(dt, mState.scene);
}

void SceneSimulatorWorkspace::renderLayout() {
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

void SceneSimulatorWorkspace::render() {
  renderLayout();

  mUIRoot.render(mState, mAssetManager, mActionExecutor, mRenderer,
                 mSceneRenderer, mEditorRenderer, mMousePickingGraph,
                 mEngineModules);
  mMouseClicked = mUIRoot.renderSceneView(
      mState, mAssetManager, mActionExecutor, mRenderer.getSceneTexture(),
      mEngineModules, mEditorCamera);

  mEngineModules.render(mState.scene);
}

void SceneSimulatorWorkspace::processShortcuts(int key, int mods) {}

void SceneSimulatorWorkspace::updateFrameData(
    rhi::RenderCommandList &commandList, u32 frameIndex) {

  mSceneRenderer.updateFrameData(mState.scene.entityDatabase,
                                 mState.activeCamera, frameIndex);
  mEditorRenderer.updateFrameData(mState.scene.entityDatabase,
                                  mState.activeCamera, mState,
                                  mAssetManager.getAssetRegistry(), frameIndex);

  if (mMousePickingGraph.isSelectionPerformedInFrame(frameIndex)) {
    auto entity = mMousePickingGraph.getSelectedEntity();
    mState.selectedEntity = entity;
  }

  if (mMouseClicked) {
    auto mousePos = mEngineModules.getWindow().getCurrentMousePosition();

    if (mEditorCamera.isWithinViewport(mousePos)) {
      auto scaledMousePos = mEditorCamera.scaleToViewport(mousePos);
      mMousePickingGraph.execute(commandList, scaledMousePos, frameIndex);
    }
    mMouseClicked = false;
  }
}

WorkspaceMatchParams SceneSimulatorWorkspace::getMatchParams() const {
  return {.type = "SceneSimulator",
          .asset = static_cast<u32>(mSceneAssetHandle),
          .assetType = AssetType::Scene};
}

void SceneSimulatorWorkspace::reload() { mUIRoot.getAssetBrowser().reload(); }

} // namespace quoll::editor
