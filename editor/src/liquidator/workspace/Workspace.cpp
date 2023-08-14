#include "liquid/core/Base.h"
#include "liquid/imgui/Imgui.h"

#include "liquidator/core/CameraLookAt.h"
#include "liquidator/editor-scene/EditorCamera.h"

#include "Workspace.h"
#include "WorkspaceLayoutRenderer.h"
#include "WorkspaceIO.h"

namespace liquid::editor {

Workspace::Workspace(Project project, AssetManager &assetManager,
                     Path scenePath)
    : mAssetManager(assetManager), mState{project},
      mActionExecutor(mState, mAssetManager.getAssetRegistry(), scenePath) {
  mActionExecutor.getSceneIO().loadScene(scenePath);

  mLayoutPath = (project.settingsPath / "layout.ini").string();
  ImGuiIO &io = ImGui::GetIO();
  io.IniFilename = mLayoutPath.c_str();
  ImGui::LoadIniSettingsFromDisk(io.IniFilename);

  auto statePath = project.settingsPath / "state.lqstate";

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
                                              "state.lqstate");
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

} // namespace liquid::editor
