#include "liquid/core/Base.h"
#include "liquid/yaml/Yaml.h"
#include "liquid/platform-tools/NativeFileDialog.h"

#include "EditorManager.h"

#include <glm/gtc/matrix_access.hpp>

namespace liquid::editor {

EditorManager::EditorManager(EditorCamera &editorCamera, const Project &project)
    : mEditorCamera(editorCamera), mProject(project) {}

void EditorManager::saveWorkspaceState(WorkspaceState &state,
                                       const std::filesystem::path &path) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  const auto &lens = scene.entityDatabase.get<PerspectiveLens>(state.camera);
  const auto &lookAt = scene.entityDatabase.get<CameraLookAt>(state.camera);

  YAML::Node node;
  node["camera"]["fov"] = lens.fovY;
  node["camera"]["near"] = lens.near;
  node["camera"]["far"] = lens.far;
  node["camera"]["eye"] = lookAt.eye;
  node["camera"]["center"] = lookAt.center;
  node["camera"]["up"] = lookAt.up;

  node["grid"]["gridLines"] = state.grid.x == 1;
  node["grid"]["axisLines"] = state.grid.y == 1;

  std::ofstream stream(path, std::ios::out);
  stream << node;
  stream.close();
}

void EditorManager::loadWorkspaceState(const std::filesystem::path &path,
                                       WorkspaceState &state) {
  state.camera = mEditorCamera.getEntity();

  std::ifstream stream(path, std::ios::in);

  if (!stream.good()) {
    return;
  }

  YAML::Node node;
  try {
    node = YAML::Load(stream);
    stream.close();
  } catch (std::exception &) {
    stream.close();
    return;
  }

  if (node["camera"].IsMap()) {
    const auto &camera = node["camera"];

    // defaults
    float fov = EditorCamera::DefaultFOV, near = EditorCamera::DefaultNear,
          far = EditorCamera::DefaultFar;
    glm::vec3 eye = EditorCamera::DefaultEye,
              center = EditorCamera::DefaultCenter,
              up = EditorCamera::DefaultUp;

    if (camera["fov"].IsScalar()) {
      fov = camera["fov"].as<float>(fov);
    }

    if (camera["near"].IsScalar()) {
      near = camera["near"].as<float>(near);
    }

    if (camera["far"].IsScalar()) {
      far = camera["far"].as<float>(far);
    }

    if (camera["eye"].IsSequence()) {
      eye = camera["eye"].as<glm::vec3>(eye);
    }

    if (camera["center"].IsSequence()) {
      center = camera["center"].as<glm::vec3>(center);
    }

    if (camera["up"].IsSequence()) {
      up = camera["up"].as<glm::vec3>(up);
    }

    auto &scene = state.mode == WorkspaceMode::Simulation
                      ? state.simulationScene
                      : state.scene;

    scene.entityDatabase.set<PerspectiveLens>(state.camera, {fov, near, far});
    scene.entityDatabase.set<CameraLookAt>(state.camera, {eye, center, up});
  }

  if (node["grid"].IsMap()) {
    bool axisLinesShown = true;
    bool gridLinesShown = true;

    const auto &grid = node["grid"];

    if (grid["axisLines"].IsScalar()) {
      axisLinesShown = grid["axisLines"].as<bool>();
    }

    if (grid["gridLines"].IsScalar()) {
      gridLinesShown = grid["gridLines"].as<bool>();
    }

    state.grid.x = static_cast<uint32_t>(gridLinesShown);
    state.grid.y = static_cast<uint32_t>(axisLinesShown);
  }
}

} // namespace liquid::editor
