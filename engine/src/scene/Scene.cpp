#include "core/Base.h"
#include "Scene.h"

namespace liquid {

SceneNode::SceneNode(Entity entity_, glm::mat4 transform_, SceneNode *parent_,
                     EntityContext &entityContext_)
    : entity(entity_), parent(parent_), entityContext(entityContext_) {
  entityContext.setComponent<TransformComponent>(entity,
                                                 {transform_, glm::mat4{1.0f}});
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

SceneNode *SceneNode::addChild(Entity entity, glm::mat4 transform) {
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

void SceneNode::setTransform(glm::mat4 transform) {
  entityContext.getComponent<TransformComponent>(entity).transformLocal =
      transform;
  dirty = true;
}

void SceneNode::setEntity(Entity entity_) {
  auto &component = entityContext.getComponent<TransformComponent>(entity);

  entityContext.setComponent<TransformComponent>(
      entity_, {component.transformLocal, component.transformWorld});
  entity = entity_;
}

void SceneNode::update(bool forceUpdate) {
  bool needsUpdate = dirty || forceUpdate;
  if (needsUpdate) {
    auto &component = entityContext.getComponent<TransformComponent>(entity);
    if (parent) {
      component.transformWorld =
          entityContext.getComponent<TransformComponent>(parent->getEntity())
              .transformWorld *
          component.transformLocal;
    } else {
      component.transformWorld = component.transformLocal;
    }

    if (entityContext.hasComponent<LightComponent>(entity)) {
      entityContext.getComponent<LightComponent>(entity).light->setPosition(
          component.transformWorld[3]);
    }

    dirty = false;
  }

  for (auto &node : children) {
    node->update(needsUpdate);
  }
}

Scene::Scene(EntityContext &entityContext_) : entityContext(entityContext_) {
  rootNode = new SceneNode(entityContext.createEntity(), glm::mat4(1.0f),
                           nullptr, entityContext);
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
