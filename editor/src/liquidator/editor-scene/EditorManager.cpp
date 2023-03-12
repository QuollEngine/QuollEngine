#include "liquid/core/Base.h"
#include "liquid/yaml/Yaml.h"
#include "liquid/platform-tools/NativeFileDialog.h"

#include "EditorManager.h"

#include <glm/gtc/matrix_access.hpp>

namespace liquid::editor {

EditorManager::EditorManager(EditorCamera &editorCamera,
                             EntityManager &entityManager,
                             const Project &project)
    : mEditorCamera(editorCamera), mEntityManager(entityManager),
      mProject(project) {}

void EditorManager::saveWorkspaceState(WorkspaceState &state,
                                       const std::filesystem::path &path) {

  auto &entityDatabase = mEntityManager.getActiveEntityDatabase();
  const auto &lens = entityDatabase.get<PerspectiveLens>(state.camera);
  const auto &lookAt = entityDatabase.get<CameraLookAt>(state.camera);

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

    auto &entityDatabase = mEntityManager.getActiveEntityDatabase();
    entityDatabase.set<PerspectiveLens>(state.camera, {fov, near, far});
    entityDatabase.set<CameraLookAt>(state.camera, {eye, center, up});
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

void EditorManager::createNewScene() {
  static constexpr glm::vec3 LightStartPos(0.0f, 5.0f, 0.0f);

  {
    LocalTransform transform{};
    transform.localPosition = LightStartPos;
    transform.localRotation =
        glm::quat(glm::vec3(0.0f, 0.0f, glm::pi<float>()));

    auto light1 =
        mEntityManager.createEmptyEntity(Entity::Null, transform, "Light");
    mEntityManager.getActiveEntityDatabase().set<DirectionalLight>(light1, {});
    mEntityManager.save(light1);
  }
}

void EditorManager::loadOrCreateScene() {
  if (!mEntityManager.loadScene()) {
    createNewScene();
  }
}

bool EditorManager::hasSkybox() {
  auto environment = mEntityManager.getActiveScene().environment;

  return mEntityManager.getActiveEntityDatabase().has<EnvironmentSkybox>(
      environment);
}

EnvironmentSkyboxType EditorManager::getSkyboxType() {
  return mEntityManager.getActiveEntityDatabase()
      .get<EnvironmentSkybox>(mEntityManager.getActiveScene().environment)
      .type;
}

const glm::vec4 &EditorManager::getSkyboxColor() {
  return mEntityManager.getActiveEntityDatabase()
      .get<EnvironmentSkybox>(mEntityManager.getActiveScene().environment)
      .color;
}

void EditorManager::setSkyboxColor(const glm::vec4 &color) {
  auto environmentEntity = mEntityManager.getActiveScene().environment;

  EnvironmentSkybox component{EnvironmentSkyboxType::Color};
  component.color = color;

  mEntityManager.getActiveEntityDatabase().set(environmentEntity, component);
  mEntityManager.saveEnvironment();
}

EnvironmentAssetHandle EditorManager::getSkyboxTexture() {
  if (!hasSkybox()) {
    return EnvironmentAssetHandle::Invalid;
  }
  auto environment = mEntityManager.getActiveScene().environment;

  return mEntityManager.getActiveEntityDatabase()
      .get<EnvironmentSkybox>(environment)
      .texture;
}

void EditorManager::setSkyboxTexture(EnvironmentAssetHandle texture) {
  auto environmentEntity = mEntityManager.getActiveScene().environment;

  EnvironmentSkybox component{EnvironmentSkyboxType::Texture};
  component.texture = texture;

  mEntityManager.getActiveEntityDatabase().set(environmentEntity, component);
  mEntityManager.saveEnvironment();
}

void EditorManager::removeSkybox() {
  auto environment = mEntityManager.getActiveScene().environment;

  if (mEntityManager.getActiveEntityDatabase().has<EnvironmentSkybox>(
          environment)) {
    mEntityManager.getActiveEntityDatabase().remove<EnvironmentSkybox>(
        environment);
  }

  mEntityManager.saveEnvironment();
}

EnvironmentLightingSource EditorManager::getEnvironmentLightingSource() {
  auto environment = mEntityManager.getActiveScene().environment;

  if (mEntityManager.getActiveEntityDatabase()
          .has<EnvironmentLightingSkyboxSource>(environment)) {
    return EnvironmentLightingSource::Skybox;
  }

  return EnvironmentLightingSource::None;
}

void EditorManager::removeEnvironmentLightingSource(bool save) {
  auto environment = mEntityManager.getActiveScene().environment;

  if (mEntityManager.getActiveEntityDatabase()
          .has<EnvironmentLightingSkyboxSource>(environment)) {
    mEntityManager.getActiveEntityDatabase()
        .remove<EnvironmentLightingSkyboxSource>(environment);
  }

  if (save) {
    mEntityManager.saveEnvironment();
  }
}

void EditorManager::setEnvironmentLightingSkyboxSource() {
  auto environment = mEntityManager.getActiveScene().environment;

  removeEnvironmentLightingSource(false);
  mEntityManager.getActiveEntityDatabase().set<EnvironmentLightingSkyboxSource>(
      environment, {});

  mEntityManager.saveEnvironment();
}

} // namespace liquid::editor
