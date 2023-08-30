#include "quoll/core/Base.h"
#include "quoll/imgui/Imgui.h"

#include "quoll/editor/core/CameraLookAt.h"
#include "quoll/editor/editor-scene/EditorCamera.h"

#include "Workspace.h"
#include "WorkspaceLayoutRenderer.h"
#include "WorkspaceIO.h"

namespace quoll::editor {

Workspace::Workspace(Project project, AssetManager &assetManager,
                     SceneAssetHandle scene, Path scenePath)
    : mAssetManager(assetManager), mState{project},
      mActionExecutor(mState, mAssetManager.getAssetRegistry(), scenePath),
      mSceneIO(mAssetManager.getAssetRegistry(), mState.scene) {
  mSceneIO.loadScene(scene);
  mActionExecutor.getSceneWriter().open(scenePath);

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

void Workspace::update() { mActionExecutor.process(); }

} // namespace quoll::editor
