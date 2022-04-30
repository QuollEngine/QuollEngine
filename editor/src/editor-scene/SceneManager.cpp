#include "liquid/core/Base.h"
#include "SceneManager.h"

#include "liquid/asset/OutputBinaryStream.h"
#include "liquid/asset/InputBinaryStream.h"
#include <json/json.hpp>

#include <glm/gtc/matrix_access.hpp>

namespace liquidator {

SceneManager::SceneManager(liquid::EntityContext &entityContext,
                           EditorCamera &editorCamera, EditorGrid &editorGrid,
                           EntityManager &entityManager)
    : mEntityContext(entityContext), mEditorCamera(editorCamera),
      mEditorGrid(editorGrid), mEntityManager(entityManager) {}

void SceneManager::saveEditorState(const std::filesystem::path &path) {
  liquid::OutputBinaryStream stream(path);

  stream.write(mEditorCamera.getFOV());
  stream.write(mEditorCamera.getNear());
  stream.write(mEditorCamera.getFar());
  stream.write(mEditorCamera.getEye());
  stream.write(mEditorCamera.getCenter());
  stream.write(mEditorCamera.getUp());

  stream.write(static_cast<uint32_t>(mEditorGrid.axisLinesShown()));
  stream.write(static_cast<uint32_t>(mEditorGrid.gridLinesShown()));
}

void SceneManager::loadEditorState(const std::filesystem::path &path) {
  liquid::InputBinaryStream stream(path);

  if (stream.good()) {
    float fov = 0.0f, near = 0.0f, far = 0.0f;
    glm::vec3 eye{}, center{}, up{};

    uint32_t axisLinesShown = 0, gridLinesShown = 0;

    stream.read(fov);
    stream.read(near);
    stream.read(far);
    stream.read(eye);
    stream.read(center);
    stream.read(up);

    stream.read(axisLinesShown);
    stream.read(gridLinesShown);

    mEditorCamera.setFOV(fov);
    mEditorCamera.setNear(near);
    mEditorCamera.setFar(far);
    mEditorCamera.setEye(eye);
    mEditorCamera.setCenter(center);
    mEditorCamera.setUp(up);

    mEditorGrid.setAxisLinesFlag(axisLinesShown == 1);
    mEditorGrid.setGridLinesFlag(gridLinesShown == 1);
  }
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
  mActiveScene = new liquid::Scene(mEntityContext);

  if (!mEntityManager.loadScene(mActiveScene->getRootNode())) {
    createNewScene();
  }
}

void SceneManager::moveCameraToEntity(liquid::Entity entity) {
  if (mEntityContext.hasComponent<liquid::TransformComponent>(entity)) {
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

} // namespace liquidator
