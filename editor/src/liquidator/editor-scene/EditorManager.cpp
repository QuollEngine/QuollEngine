#include "liquid/core/Base.h"
#include "liquid/yaml/Yaml.h"
#include "liquid/platform-tools/NativeFileDialog.h"

#include "EditorManager.h"
#include "liquidator/core/GameExporter.h"

#include <glm/gtc/matrix_access.hpp>

namespace liquid::editor {

EditorManager::EditorManager(EditorCamera &editorCamera, EditorGrid &editorGrid,
                             EntityManager &entityManager,
                             const Project &project)
    : mEditorCamera(editorCamera), mEditorGrid(editorGrid),
      mEntityManager(entityManager), mProject(project) {}

void EditorManager::saveEditorState(const std::filesystem::path &path) {
  YAML::Node node;
  node["camera"]["fov"] = mEditorCamera.getFOV();
  node["camera"]["near"] = mEditorCamera.getNear();
  node["camera"]["far"] = mEditorCamera.getFar();
  node["camera"]["eye"] = mEditorCamera.getEye();
  node["camera"]["center"] = mEditorCamera.getCenter();
  node["camera"]["up"] = mEditorCamera.getUp();

  node["grid"]["axisLines"] = mEditorGrid.axisLinesShown();
  node["grid"]["gridLines"] = mEditorGrid.gridLinesShown();

  std::ofstream stream(path, std::ios::out);
  stream << node;
  stream.close();
}

void EditorManager::loadEditorState(const std::filesystem::path &path) {
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

  stream.close();
  if (node["camera"].IsMap()) {
    const auto &camera = node["camera"];

    // defaults
    float fov = EditorCamera::DefaultFOV, near = EditorCamera::DefaultNear,
          far = EditorCamera::DefaultFar;
    glm::vec3 eye = EditorCamera::DefaultEye,
              center = EditorCamera::DefaultCenter,
              up = EditorCamera::DefaultUp;

    if (camera["fov"].IsScalar()) {
      fov = camera["fov"].as<float>();
    }

    if (camera["near"].IsScalar()) {
      near = camera["near"].as<float>();
    }

    if (camera["far"].IsScalar()) {
      far = camera["far"].as<float>();
    }

    if (camera["eye"].IsSequence()) {
      eye = camera["eye"].as<glm::vec3>();
    }

    if (camera["center"].IsSequence()) {
      center = camera["center"].as<glm::vec3>();
    }

    if (camera["up"].IsSequence()) {
      up = camera["up"].as<glm::vec3>();
    }

    mEditorCamera.setFOV(fov);
    mEditorCamera.setNear(near);
    mEditorCamera.setFar(far);
    mEditorCamera.setEye(eye);
    mEditorCamera.setCenter(center);
    mEditorCamera.setUp(up);
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

    mEditorGrid.setAxisLinesFlag(axisLinesShown);
    mEditorGrid.setGridLinesFlag(gridLinesShown);
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
  mEditorCamera.reset();

  if (!mEntityManager.loadScene()) {
    createNewScene();
  }
}

void EditorManager::moveCameraToEntity(Entity entity) {
  if (!mEntityManager.getActiveEntityDatabase().has<LocalTransform>(entity)) {
    return;
  }

  auto &transformComponent =
      mEntityManager.getActiveEntityDatabase().get<WorldTransform>(entity);

  const auto &translation =
      glm::vec3(glm::column(transformComponent.worldTransform, 3));

  static constexpr glm::vec3 DistanceFromCenter{0.0f, 0.0f, 10.0f};

  mEditorCamera.reset();
  mEditorCamera.setCenter(translation);
  mEditorCamera.setEye(translation - DistanceFromCenter);
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

void EditorManager::setTransformOperation(
    TransformOperation transformOperation) {
  mTransformOperation = transformOperation;
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

void EditorManager::startGameExport() {
  platform_tools::NativeFileDialog nativeFileDialog;
  auto path = nativeFileDialog.getFilePathFromCreateDialog({});

  GameExporter exporter;
  exporter.exportGame(mProject, path);
}

} // namespace liquid::editor
