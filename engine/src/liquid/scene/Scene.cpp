#include "liquid/core/Base.h"
#include "Scene.h"

namespace liquid {

SceneNode::SceneNode(Entity entity_, const TransformComponent &transform,
                     SceneNode *parent_, EntityContext &entityContext_)
    : entity(entity_), parent(parent_), entityContext(entityContext_) {
  TransformComponent newTransform = transform;
  if (parent) {
    newTransform.parent = parent->getEntity();
  }
  entityContext.setComponent<TransformComponent>(entity, newTransform);
}

SceneNode::~SceneNode() {
  if (entityContext.hasComponent<TransformComponent>(entity)) {
    entityContext.deleteComponent<TransformComponent>(entity);
  }

  for (auto &x : children) {
    delete x;
  }

  children.clear();
}

SceneNode *SceneNode::addChild(Entity entity,
                               const TransformComponent &transform) {
  addChild(new SceneNode(entity, transform, this, entityContext));
  return children.back();
}

void SceneNode::addChild(SceneNode *node) {
  node->parent = this;
  children.push_back(node);
}

void SceneNode::removeChild(SceneNode *node) {
  auto it = std::find(children.begin(), children.end(), node);

  if (it != children.end()) {
    children.erase(it);
    delete node;
  }
}

void SceneNode::setEntity(Entity entity_) {
  auto &component = entityContext.getComponent<TransformComponent>(entity);

  entityContext.setComponent<TransformComponent>(entity_, component);
  entity = entity_;
}

void SceneNode::update() {
  LIQUID_PROFILE_EVENT("SceneNode::Update");
  auto &component = entityContext.getComponent<TransformComponent>(entity);

  glm::mat4 identity{1.0f};
  glm::mat4 localTransform = glm::translate(identity, component.localPosition) *
                             glm::toMat4(component.localRotation) *
                             glm::scale(identity, component.localScale);

  if (parent) {
    component.worldTransform =
        entityContext.getComponent<TransformComponent>(parent->getEntity())
            .worldTransform *
        localTransform;
  } else {
    component.worldTransform = localTransform;
  }

  if (entityContext.hasComponent<LightComponent>(entity)) {
    entityContext.getComponent<LightComponent>(entity).light->setPosition(
        component.worldTransform[3]);
  }

  for (auto &node : children) {
    node->update();
  }
}

Scene::Scene(EntityContext &entityContext_) : entityContext(entityContext_) {
  rootNode =
      new SceneNode(entityContext.createEntity(), {}, nullptr, entityContext);
}

Scene::~Scene() {
  if (rootNode) {
    delete rootNode;
    rootNode = nullptr;
  }
}

void Scene::setActiveCamera(Entity camera) {
  LIQUID_ASSERT(entityContext.hasComponent<CameraComponent>(camera),
                "Entity " + std::to_string(camera) +
                    " does not have a camera component");

  cameraEntity = camera;
}

const SharedPtr<Camera> &Scene::getActiveCamera() {
  LIQUID_ASSERT(entityContext.hasComponent<CameraComponent>(cameraEntity),
                "Entity " + std::to_string(cameraEntity) +
                    " does not have a camera component");

  return entityContext.getComponent<CameraComponent>(cameraEntity).camera;
}

void Scene::update() { rootNode->update(); }

} // namespace liquid
