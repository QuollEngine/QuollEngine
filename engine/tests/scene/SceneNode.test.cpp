#include "liquid/core/Base.h"
#include "liquid/scene/Scene.h"

#include "../mocks/TestResourceAllocator.h"
#include <gtest/gtest.h>

glm::mat4 getLocalTransform(const liquid::TransformComponent &transform) {
  return glm::translate(glm::mat4(1.0f), transform.localPosition) *
         glm::toMat4(transform.localRotation) *
         glm::scale(glm::mat4(1.0f), transform.localScale);
}

TEST(SceneTest, CreatesNodeWithIdentityWorldTransform) {
  liquid::EntityContext context;
  liquid::TransformComponent transform{glm::vec3{1.0f, 0.0f, 1.0f},
                                       glm::quat{1.0f, 0.5f, 0.5f, 0.5f},
                                       glm::vec3{0.5f, 0.5f, 0.5f}};

  auto entity = context.createEntity();
  liquid::SceneNode sceneNode(entity, transform, nullptr, context);

  EXPECT_EQ(sceneNode.getEntity(), entity);
  EXPECT_EQ(sceneNode.getParent(), nullptr);

  EXPECT_EQ(
      context.getComponent<liquid::TransformComponent>(entity).localPosition,
      transform.localPosition);
  EXPECT_EQ(
      context.getComponent<liquid::TransformComponent>(entity).localRotation,
      transform.localRotation);
  EXPECT_EQ(context.getComponent<liquid::TransformComponent>(entity).localScale,
            transform.localScale);
  EXPECT_EQ(
      context.getComponent<liquid::TransformComponent>(entity).worldTransform,
      glm::mat4(1.0f));

  EXPECT_EQ(sceneNode.getWorldTransform(), glm::mat4(1.0f));
}

TEST(SceneTest, AddsChildWithLocalTransform) {
  liquid::EntityContext context;
  liquid::Scene scene(context);
  liquid::SceneNode sceneNode(context.createEntity(), {}, nullptr, context);
  sceneNode.addChild(context.createEntity());
  sceneNode.addChild(context.createEntity());
  sceneNode.addChild(context.createEntity());

  EXPECT_EQ(sceneNode.getChildren().size(), 3);
  for (auto &i : sceneNode.getChildren()) {
    EXPECT_EQ(i->getParent(), &sceneNode);
  }
}

TEST(SceneTest, AddsSceneNode) {
  liquid::EntityContext context;
  liquid::Scene scene(context);
  liquid::SceneNode sceneNode(context.createEntity(), {}, nullptr, context);

  std::array<liquid::SceneNode *, 2> entityList{
      new liquid::SceneNode{context.createEntity(), {}, nullptr, context},
      new liquid::SceneNode{context.createEntity(), {}, nullptr, context}};

  sceneNode.addChild(entityList.at(0));
  sceneNode.addChild(entityList.at(1));

  EXPECT_EQ(sceneNode.getChildren().size(), 2);
  const auto &children = sceneNode.getChildren();
  for (size_t i = 0; i < children.size(); ++i) {
    EXPECT_EQ(sceneNode.getChildren().at(i)->getParent(), &sceneNode);
    EXPECT_EQ(sceneNode.getChildren().at(i)->getEntity(),
              entityList.at(i)->getEntity());
  }
}

TEST(SceneTest, SetsEntity) {
  liquid::EntityContext context;
  auto entity = context.createEntity();

  liquid::TransformComponent transform;
  transform.localPosition = glm::vec3(1.0f, 0.0f, 0.0);
  liquid::SceneNode root(context.createEntity(), {}, nullptr, context);
  auto *sceneNode = root.addChild(entity, transform);

  root.update();

  EXPECT_EQ(sceneNode->getEntity(), entity);
  EXPECT_EQ(
      context.getComponent<liquid::TransformComponent>(entity).localPosition,
      transform.localPosition);
  EXPECT_EQ(
      context.getComponent<liquid::TransformComponent>(entity).worldTransform,
      getLocalTransform(transform));

  auto newEntity = context.createEntity();
  sceneNode->setEntity(newEntity);

  EXPECT_NE(newEntity, entity);
  EXPECT_EQ(sceneNode->getEntity(), newEntity);
  EXPECT_EQ(
      context.getComponent<liquid::TransformComponent>(newEntity).localPosition,
      transform.localPosition);
  EXPECT_EQ(context.getComponent<liquid::TransformComponent>(newEntity)
                .worldTransform,
            getLocalTransform(transform));
}

TEST(SceneTest, UpdateSetsLocalMatrixToWorldIfNoParent) {
  liquid::EntityContext context;

  liquid::TransformComponent transform;
  transform.localPosition = glm::vec3(1.0f, 0.0f, 0.0);

  liquid::SceneNode sceneNode(context.createEntity(), transform, nullptr,
                              context);
  sceneNode.update();

  EXPECT_EQ(
      context.getComponent<liquid::TransformComponent>(sceneNode.getEntity())
          .worldTransform,
      getLocalTransform(transform));
}

TEST(SceneTest, UpdatesChildrenWithParent) {
  liquid::EntityContext context;

  liquid::TransformComponent parentTransform;
  parentTransform.localPosition = glm::vec3(1.0f);

  liquid::TransformComponent childTransform;
  childTransform.localRotation =
      glm::angleAxis(glm::radians(15.f), glm::vec3(0.0f, 1.0f, 0.0f));

  liquid::SceneNode parentNode(context.createEntity(), parentTransform, nullptr,
                               context);
  auto *childNode = parentNode.addChild(context.createEntity(), childTransform);

  parentNode.update();
  EXPECT_EQ(childNode->getWorldTransform(),
            getLocalTransform(parentTransform) *
                getLocalTransform(childTransform));
}

TEST(SceneTest, UpdatesLightIfExists) {
  liquid::EntityContext context;

  liquid::TransformComponent transform;
  transform.localPosition = glm::vec3(1.0f, 0.5f, 1.0f);

  auto entity = context.createEntity();

  liquid::Scene scene(context);

  context.setComponent<liquid::LightComponent>(
      entity, {std::make_shared<liquid::Light>(
                  liquid::Light::DIRECTIONAL, glm::vec3{}, glm::vec4{}, 1.0f)});

  liquid::SceneNode sceneNode(entity, transform, nullptr, context);
  sceneNode.update();

  EXPECT_EQ(
      context.getComponent<liquid::LightComponent>(entity).light->getPosition(),
      transform.localPosition);
}

TEST(SceneTest, DeletesChildIfExists) {
  liquid::EntityContext context;

  liquid::SceneNode *node =
      new liquid::SceneNode(context.createEntity(), {}, nullptr, context);

  auto e1 = context.createEntity();
  auto e2 = context.createEntity();

  auto *child1 = node->addChild(e1);
  auto *child2 = node->addChild(e2);

  EXPECT_EQ(node->getChildren().size(), 2);
  EXPECT_EQ(node->getChildren().at(0), child1);
  EXPECT_EQ(node->getChildren().at(1), child2);

  node->removeChild(child1);
  EXPECT_EQ(node->getChildren().size(), 1);
  EXPECT_EQ(node->getChildren().at(0), child2);
  EXPECT_FALSE(context.hasComponent<liquid::TransformComponent>(e1));

  delete node;
}

TEST(SceneTest, DoesNotDeleteNodeThatsNotAChild) {
  liquid::EntityContext context;

  liquid::SceneNode *node =
      new liquid::SceneNode(context.createEntity(), {}, nullptr, context);

  auto e2 = context.createEntity();
  liquid::SceneNode *node2 = new liquid::SceneNode(e2, {}, nullptr, context);

  node->removeChild(node2);

  EXPECT_TRUE(context.hasComponent<liquid::TransformComponent>(e2));

  delete node;
  delete node2;
}