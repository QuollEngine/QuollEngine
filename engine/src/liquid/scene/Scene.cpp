#include "liquid/core/Base.h"
#include "Scene.h"

namespace liquid {

SceneNode::SceneNode(Entity entity, const TransformComponent &transform,
                     SceneNode *parent, EntityContext &entityContext)
    : mEntity(entity), mParent(parent), mEntityContext(entityContext) {
  TransformComponent newTransform = transform;
  if (mParent) {
    newTransform.parent = mParent->getEntity();
  }
  mEntityContext.setComponent<TransformComponent>(mEntity, newTransform);
}

SceneNode::~SceneNode() {
  if (mEntityContext.hasComponent<TransformComponent>(mEntity)) {
    mEntityContext.deleteComponent<TransformComponent>(mEntity);
  }

  for (auto &x : mChildren) {
    delete x;
  }

  mChildren.clear();
}

SceneNode *SceneNode::addChild(Entity entity,
                               const TransformComponent &transform) {
  addChild(new SceneNode(entity, transform, this, mEntityContext));
  return mChildren.back();
}

void SceneNode::addChild(SceneNode *node) {
  node->mParent = this;
  mChildren.push_back(node);
}

void SceneNode::removeChild(SceneNode *node) {
  auto it = std::find(mChildren.begin(), mChildren.end(), node);

  if (it != mChildren.end()) {
    mChildren.erase(it);
    delete node;
  }
}

void SceneNode::setEntity(Entity entity) {
  auto &component = mEntityContext.getComponent<TransformComponent>(mEntity);

  mEntityContext.setComponent<TransformComponent>(entity, component);
  mEntity = entity;
}

void SceneNode::update() {
  LIQUID_PROFILE_EVENT("SceneNode::Update");
  auto &component = mEntityContext.getComponent<TransformComponent>(mEntity);

  glm::mat4 identity{1.0f};
  glm::mat4 localTransform = glm::translate(identity, component.localPosition) *
                             glm::toMat4(component.localRotation) *
                             glm::scale(identity, component.localScale);

  if (mParent) {
    component.worldTransform =
        mEntityContext.getComponent<TransformComponent>(mParent->getEntity())
            .worldTransform *
        localTransform;
  } else {
    component.worldTransform = localTransform;
  }

  if (mEntityContext.hasComponent<LightComponent>(mEntity)) {
    mEntityContext.getComponent<LightComponent>(mEntity).light->setPosition(
        component.worldTransform[3]);
  }

  for (auto &node : mChildren) {
    node->update();
  }
}

Scene::Scene(EntityContext &entityContext) : mEntityContext(entityContext) {
  mRootNode =
      new SceneNode(mEntityContext.createEntity(), {}, nullptr, mEntityContext);
}

Scene::~Scene() {
  if (mRootNode) {
    delete mRootNode;
    mRootNode = nullptr;
  }
}

void Scene::setActiveCamera(Entity camera) {
  LIQUID_ASSERT(mEntityContext.hasComponent<CameraComponent>(camera),
                "Entity " + std::to_string(camera) +
                    " does not have a camera component");

  mCameraEntity = camera;
}

const SharedPtr<Camera> &Scene::getActiveCamera() {
  LIQUID_ASSERT(mEntityContext.hasComponent<CameraComponent>(mCameraEntity),
                "Entity " + std::to_string(mCameraEntity) +
                    " does not have a camera component");

  return mEntityContext.getComponent<CameraComponent>(mCameraEntity).camera;
}

void Scene::update() { mRootNode->update(); }

} // namespace liquid
