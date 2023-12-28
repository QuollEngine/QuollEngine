#include "quoll/core/Base.h"
#include "quoll/yaml/Yaml.h"
#include "quoll/platform/tools/FileDialog.h"
#include "quoll/scene/PerspectiveLens.h"

#include "quoll/editor/core/CameraLookAt.h"
#include "quoll/editor/scene/core/EditorCamera.h"

#include "WorkspaceIO.h"
#include <glm/gtc/matrix_access.hpp>

namespace quoll::editor {

void WorkspaceIO::saveWorkspaceState(WorkspaceState &state,
                                     const std::filesystem::path &path) {
  auto &scene = state.scene;

  const auto &lens = scene.entityDatabase.get<PerspectiveLens>(state.camera);
  const auto &lookAt = scene.entityDatabase.get<CameraLookAt>(state.camera);

  YAML::Node node;
  node["camera"]["near"] = lens.near;
  node["camera"]["far"] = lens.far;
  node["camera"]["sensorSize"] = lens.sensorSize;
  node["camera"]["focalLength"] = lens.focalLength;
  node["camera"]["eye"] = lookAt.eye;
  node["camera"]["center"] = lookAt.center;
  node["camera"]["up"] = lookAt.up;

  node["grid"]["gridLines"] = state.grid.x == 1;
  node["grid"]["axisLines"] = state.grid.y == 1;

  std::ofstream stream(path, std::ios::out);
  stream << node;
  stream.close();
}

void WorkspaceIO::loadWorkspaceState(WorkspaceState &state,
                                     const std::filesystem::path &path) {
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
    auto near = EditorCamera::DefaultNear;
    auto far = EditorCamera::DefaultFar;
    auto sensorSize = EditorCamera::DefaultSensorSize;
    auto focalLength = EditorCamera::DefaultFocalLength;
    auto eye = EditorCamera::DefaultEye;
    auto center = EditorCamera::DefaultCenter;
    auto up = EditorCamera::DefaultUp;

    if (camera["near"] && camera["near"].IsScalar()) {
      near = camera["near"].as<f32>(near);
    }

    if (camera["far"] && camera["far"].IsScalar()) {
      far = camera["far"].as<f32>(far);
    }

    if (camera["sensorSize"] && camera["sensorSize"].IsSequence()) {
      sensorSize = camera["sensorSize"].as<glm::vec2>(sensorSize);
    }

    if (camera["focalLength"] && camera["focalLength"].IsScalar()) {
      focalLength = camera["sensorSize"].as<f32>(focalLength);
    }

    if (camera["eye"] && camera["eye"].IsSequence()) {
      eye = camera["eye"].as<glm::vec3>(eye);
    }

    if (camera["center"] && camera["center"].IsSequence()) {
      center = camera["center"].as<glm::vec3>(center);
    }

    if (camera["up"] && camera["up"].IsSequence()) {
      up = camera["up"].as<glm::vec3>(up);
    }

    auto &scene = state.scene;

    PerspectiveLens lens{};
    lens.near = near;
    lens.far = far;
    lens.sensorSize = sensorSize;
    lens.focalLength = focalLength;

    scene.entityDatabase.set<PerspectiveLens>(state.camera, lens);
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

    state.grid.x = static_cast<u32>(gridLinesShown);
    state.grid.y = static_cast<u32>(axisLinesShown);
  }
}

} // namespace quoll::editor
