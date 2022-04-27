#include "liquid/core/Base.h"
#include "SceneManager.h"

#include "liquid/asset/OutputBinaryStream.h"
#include "liquid/asset/InputBinaryStream.h"

namespace liquidator {

SceneManager::SceneManager(liquid::EntityContext &entityContext,
                           EditorCamera &editorCamera, EditorGrid &editorGrid)
    : mEntityContext(entityContext), mEditorCamera(editorCamera),
      mEditorGrid(editorGrid) {}

liquid::SceneNode *SceneManager::createEntityAtView() {
  const auto &viewMatrix =
      mEntityContext
          .getComponent<liquid::CameraComponent>(mEditorCamera.getCamera())
          .camera->getViewMatrix();

  constexpr glm::vec3 distanceFromEye = {0.0f, 0.0f, -10.0f};
  const auto &invViewMatrix = glm::inverse(viewMatrix);
  const auto &orientation = invViewMatrix * glm::translate(distanceFromEye);

  liquid::TransformComponent transform;
  transform.localPosition = orientation[3];

  auto entity = mEntityContext.createEntity();
  mEntityContext.setComponent<liquid::DebugComponent>(entity, {});

  auto *node = mActiveScene->getRootNode()->addChild(entity, transform);

  return node;
}

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
  if (mActiveScene) {
    delete mActiveScene;
    mEntityContext.destroy();
  }

  mEditorCamera.reset();
  mActiveScene = new liquid::Scene(mEntityContext);

  {
    auto light1 = mEntityContext.createEntity();
    mEntityContext.setComponent<liquid::NameComponent>(light1, {"Light"});
    mEntityContext.setComponent<liquid::LightComponent>(
        light1, {std::make_shared<liquid::Light>(
                    liquid::Light::DIRECTIONAL,
                    glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, 1.0f)});
    mEntityContext.setComponent<liquid::DebugComponent>(light1, {});

    liquid::TransformComponent transform{};
    transform.localPosition = LIGHT_START_POS;
    transform.localRotation =
        glm::quat(glm::vec3(0.0f, 0.0f, glm::pi<float>()));

    mActiveScene->getRootNode()->addChild(light1, transform);
  }
}

} // namespace liquidator
