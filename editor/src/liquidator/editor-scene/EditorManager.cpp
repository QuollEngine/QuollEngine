#include "liquid/core/Base.h"
#include "liquid/yaml/Yaml.h"

#include "EditorManager.h"

#include <glm/gtc/matrix_access.hpp>

namespace liquidator {

EditorManager::EditorManager(EditorCamera &editorCamera, EditorGrid &editorGrid,
                             EntityManager &entityManager)
    : mEditorCamera(editorCamera), mEditorGrid(editorGrid),
      mEntityManager(entityManager) {}

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

void EditorManager::setActiveCamera(liquid::Entity camera) {
  if (!mEntityManager.getActiveEntityDatabase()
           .hasComponent<liquid::CameraComponent>(camera)) {
    return;
  }

  mCameraEntity = camera;
}

void EditorManager::switchToEditorCamera() {
  mCameraEntity = mEditorCamera.getCamera();
}

void EditorManager::createNewScene() {
  static constexpr glm::vec3 LightStartPos(0.0f, 5.0f, 0.0f);

  {
    liquid::LocalTransformComponent transform{};
    transform.localPosition = LightStartPos;
    transform.localRotation =
        glm::quat(glm::vec3(0.0f, 0.0f, glm::pi<float>()));

    auto light1 = mEntityManager.createEmptyEntity(liquid::EntityNull,
                                                   transform, "Light");
    mEntityManager.getActiveEntityDatabase()
        .setComponent<liquid::DirectionalLightComponent>(light1, {});
    mEntityManager.save(light1);
  }
}

void EditorManager::loadOrCreateScene() {
  mEditorCamera.reset();
  mCameraEntity = mEditorCamera.getCamera();

  if (!mEntityManager.loadScene()) {
    createNewScene();
  }
}

void EditorManager::moveCameraToEntity(liquid::Entity entity) {
  if (!mEntityManager.getActiveEntityDatabase()
           .hasComponent<liquid::LocalTransformComponent>(entity)) {
    return;
  }

  auto &transformComponent =
      mEntityManager.getActiveEntityDatabase()
          .getComponent<liquid::WorldTransformComponent>(entity);

  const auto &translation =
      glm::vec3(glm::column(transformComponent.worldTransform, 3));

  static constexpr glm::vec3 DistanceFromCenter{0.0f, 0.0f, 10.0f};

  mEditorCamera.reset();
  mEditorCamera.setCenter(translation);
  mEditorCamera.setEye(translation - DistanceFromCenter);
}

bool EditorManager::hasEnvironment() {
  return mEntityManager.getActiveEntityDatabase()
      .hasComponent<liquid::EnvironmentComponent>(mEnvironmentEntity);
}

liquid::EnvironmentComponent &EditorManager::getEnvironment() {
  if (!mEntityManager.getActiveEntityDatabase().hasEntity(mEnvironmentEntity)) {
    mEnvironmentEntity =
        mEntityManager.getActiveEntityDatabase().createEntity();
    mEntityManager.getActiveEntityDatabase()
        .setComponent<liquid::EnvironmentComponent>(mEnvironmentEntity, {});
  }

  return mEntityManager.getActiveEntityDatabase()
      .getComponent<liquid::EnvironmentComponent>(mEnvironmentEntity);
}

} // namespace liquidator
