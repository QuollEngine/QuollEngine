#include "liquid/core/Base.h"
#include "SceneManager.h"
#include "liquid/yaml/Yaml.h"

#include <glm/gtc/matrix_access.hpp>

namespace liquidator {

SceneManager::SceneManager(liquid::EntityContext &entityContext,
                           EditorCamera &editorCamera, EditorGrid &editorGrid,
                           EntityManager &entityManager)
    : mEntityContext(entityContext), mEditorCamera(editorCamera),
      mEditorGrid(editorGrid), mEntityManager(entityManager) {}

void SceneManager::saveEditorState(const std::filesystem::path &path) {
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

void SceneManager::loadEditorState(const std::filesystem::path &path) {
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
    float fov = EditorCamera::DEFAULT_FOV, near = EditorCamera::DEFAULT_NEAR,
          far = EditorCamera::DEFAULT_FAR;
    glm::vec3 eye = EditorCamera::DEFAULT_EYE,
              center = EditorCamera::DEFAULT_CENTER,
              up = EditorCamera::DEFAULT_UP;

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

void SceneManager::setCamera(liquid::Entity camera) {
  if (!mEntityContext.hasComponent<liquid::CameraComponent>(camera)) {
    return;
  }

  mCameraEntity = camera;
}

void SceneManager::switchToEditorCamera() {
  mCameraEntity = mEditorCamera.getCamera();
}

void SceneManager::createNewScene() {
  static constexpr glm::vec3 LIGHT_START_POS(0.0f, 5.0f, 0.0f);

  {
    liquid::TransformComponent transform{};
    transform.localPosition = LIGHT_START_POS;
    transform.localRotation =
        glm::quat(glm::vec3(0.0f, 0.0f, glm::pi<float>()));

    auto light1 =
        mEntityManager
            .createEmptyEntity(mActiveScene->getRootNode(), transform, "Light")
            ->getEntity();
    mEntityContext.setComponent<liquid::LightComponent>(
        light1, {std::make_shared<liquid::Light>(
                    liquid::LightType::Directional,
                    glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, 1.0f)});
    mEntityContext.setComponent<liquid::DebugComponent>(light1, {});
    mEntityManager.save(light1);
  }
}

void SceneManager::loadOrCreateScene() {
  mEditorCamera.reset();
  mCameraEntity = mEditorCamera.getCamera();

  mActiveScene = new liquid::Scene(mEntityContext);

  if (!mEntityManager.loadScene(mActiveScene->getRootNode())) {
    createNewScene();
  }
}

void SceneManager::moveCameraToEntity(liquid::Entity entity) {
  if (!mEntityContext.hasComponent<liquid::TransformComponent>(entity)) {
    return;
  }

  auto &transformComponent =
      mEntityContext.getComponent<liquid::TransformComponent>(entity);

  const auto &translation =
      glm::vec3(glm::column(transformComponent.worldTransform, 3));

  constexpr glm::vec3 distanceFromCenter{0.0f, 0.0f, 10.0f};

  mEditorCamera.reset();
  mEditorCamera.setCenter(translation);
  mEditorCamera.setEye(translation - distanceFromCenter);
}

bool SceneManager::hasEnvironment() {
  return mEntityContext.hasComponent<liquid::EnvironmentComponent>(
      mEnvironmentEntity);
}

liquid::EnvironmentComponent &SceneManager::getEnvironment() {
  if (!mEntityContext.hasEntity(mEnvironmentEntity)) {
    mEnvironmentEntity = mEntityContext.createEntity();
    mEntityContext.setComponent<liquid::EnvironmentComponent>(
        mEnvironmentEntity, {});
  }

  return mEntityContext.getComponent<liquid::EnvironmentComponent>(
      mEnvironmentEntity);
}

} // namespace liquidator
