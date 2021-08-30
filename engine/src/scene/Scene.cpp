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
  children.push_back(new SceneNode(entity, transform, this, entityContext));
  return children.back();
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

void Scene::setActiveCamera(Camera *camera_) { camera = camera_; }

void Scene::update() { rootNode->update(); }

} // namespace liquid
