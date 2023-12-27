#include "quoll/core/Base.h"
#include "quoll/imgui/Imgui.h"

#include "quoll/editor/core/CameraLookAt.h"
#include "quoll/editor/workspace/WorkspaceLayoutRenderer.h"
#include "quoll/editor/workspace/WorkspaceIO.h"
#include "quoll/editor/actions/TypedActionCreator.h"
#include "quoll/editor/actions/HistoryActions.h"
#include "quoll/editor/actions/SpawnEntityActions.h"

#include "core/EditorCamera.h"
#include "SceneSimulatorWorkspace.h"

namespace quoll::editor {

SceneSimulatorWorkspace::SceneSimulatorWorkspace(
    Project project, AssetManager &assetManager, SceneAssetHandle scene,
    Scene &sourceScene, Renderer &renderer, SceneRenderer &sceneRenderer,
    EditorRenderer &editorRenderer, MousePickingGraph &mousePickingGraph,
    SceneSimulator &editorSimulator)
    : mAssetManager(assetManager), mState{project},
      mActionExecutor(mState, mAssetManager.getAssetRegistry()),
      mSceneAssetHandle(scene),
      mSceneWriter(mState.simulationScene, mAssetManager.getAssetRegistry()),
      mSceneIO(mAssetManager.getAssetRegistry(), mState.simulationScene),
      mRenderer(renderer), mSceneRenderer(sceneRenderer),
      mEditorRenderer(editorRenderer), mMousePickingGraph(mousePickingGraph),
      mEditorSimulator(editorSimulator) {

  sourceScene.entityDatabase.duplicate(mState.simulationScene.entityDatabase);
  mState.simulationScene.dummyCamera = sourceScene.dummyCamera;
  mState.simulationScene.activeEnvironment = sourceScene.activeEnvironment;
  mState.simulationScene.dummyEnvironment = sourceScene.dummyEnvironment;
  mState.mode = WorkspaceMode::Simulation;

  if (sourceScene.entityDatabase.has<Camera>(sourceScene.activeCamera)) {
    mState.simulationScene.activeCamera = sourceScene.activeCamera;
    mState.activeCamera = sourceScene.activeCamera;
  } else {
    mState.simulationScene.activeCamera = sourceScene.dummyCamera;
    mState.activeCamera = sourceScene.dummyCamera;
  }

  mState.camera = mState.activeCamera;

  mRequiresDockspaceInit = true;

  ImGuiIO &io = ImGui::GetIO();
  io.IniFilename = nullptr;

  mEditorSimulator.observeChanges(mState.simulationScene.entityDatabase);
}

SceneSimulatorWorkspace::~SceneSimulatorWorkspace() {
  mEditorSimulator.cleanupSimulationDatabase(
      mState.simulationScene.entityDatabase);
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

void SceneSimulatorWorkspace::update(f32 dt) {
  mActionExecutor.process();
  mEditorSimulator.updateSimulation(dt, mState);
}

void SceneSimulatorWorkspace::render() {
  renderLayout();

  mUIRoot.render(mState, mAssetManager, mActionExecutor, mRenderer,
                 mSceneRenderer, mEditorRenderer, mMousePickingGraph,
                 mEditorSimulator);
  mMouseClicked =
      mUIRoot.renderSceneView(mState, mAssetManager, mActionExecutor,
                              mRenderer.getSceneTexture(), mEditorSimulator);

  mEditorSimulator.render(mState.simulationScene.entityDatabase);
}

void SceneSimulatorWorkspace::processShortcuts(int key, int mods) {}

void SceneSimulatorWorkspace::updateFrameData(
    rhi::RenderCommandList &commandList, u32 frameIndex) {

  mSceneRenderer.updateFrameData(mState.simulationScene.entityDatabase,
                                 mState.activeCamera, frameIndex);
  mEditorRenderer.updateFrameData(mState.simulationScene.entityDatabase,
                                  mState.activeCamera, mState,
                                  mAssetManager.getAssetRegistry(), frameIndex);

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

WorkspaceMatchParams SceneSimulatorWorkspace::getMatchParams() const {
  return {.type = "SceneSimulator",
          .asset = static_cast<u32>(mSceneAssetHandle),
          .assetType = AssetType::Scene};
}

void SceneSimulatorWorkspace::reload() { mUIRoot.getAssetBrowser().reload(); }

} // namespace quoll::editor
