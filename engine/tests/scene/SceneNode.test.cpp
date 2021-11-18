#include "core/Base.h"
#include "scene/Scene.h"

#include "../mocks/TestResourceAllocator.h"
#include <gtest/gtest.h>

liquid::SharedPtr<liquid::MeshInstance> emptyMeshInstance = nullptr;

TEST(SceneTest, CreatesNodeWithIdentityWorldTransform) {
  liquid::EntityContext context;
  auto local = glm::translate(glm::mat4{1.0f}, {1.0f, 1.0f, 1.0f});
  auto entity = context.createEntity();
  liquid::SceneNode sceneNode(entity, local, nullptr, context);

  EXPECT_EQ(sceneNode.getEntity(), entity);
  EXPECT_EQ(sceneNode.getParent(), nullptr);

  EXPECT_TRUE(
      context.getComponent<liquid::TransformComponent>(entity).transformLocal ==
      local);
  EXPECT_TRUE(
      context.getComponent<liquid::TransformComponent>(entity).transformWorld ==
      glm::mat4{1.0f});

  EXPECT_TRUE(sceneNode.getWorldTransform() == glm::mat4{1.0f});
  EXPECT_FALSE(sceneNode.getWorldTransform() == local);
}

TEST(SceneTest, AddsChildWithLocalTransform) {
  liquid::EntityContext context;
  liquid::Scene scene(context);
  liquid::SceneNode sceneNode(context.createEntity(), glm::mat4{1.0}, nullptr,
                              context);
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
  liquid::SceneNode sceneNode(context.createEntity(), glm::mat4{1.0}, nullptr,
                              context);

  std::array<liquid::SceneNode *, 2> entityList{
      new liquid::SceneNode{context.createEntity(), glm::mat4{1.0}, nullptr,
                            context},
      new liquid::SceneNode{context.createEntity(), glm::mat4{1.0}, nullptr,
                            context}};

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
  liquid::SceneNode root(context.createEntity(), glm::mat4{5.0f}, nullptr,
                         context);
  auto *sceneNode = root.addChild(entity);

  root.update();

  EXPECT_EQ(sceneNode->getEntity(), entity);
  EXPECT_TRUE(
      context.getComponent<liquid::TransformComponent>(entity).transformLocal ==
      glm::mat4{1.0f});
  EXPECT_TRUE(
      context.getComponent<liquid::TransformComponent>(entity).transformWorld ==
      glm::mat4{5.0f});

  auto newEntity = context.createEntity();
  sceneNode->setEntity(newEntity);

  EXPECT_NE(newEntity, entity);
  EXPECT_EQ(sceneNode->getEntity(), newEntity);
  EXPECT_TRUE(context.getComponent<liquid::TransformComponent>(newEntity)
                  .transformLocal == glm::mat4{1.0f});
  EXPECT_TRUE(context.getComponent<liquid::TransformComponent>(newEntity)
                  .transformWorld == glm::mat4{5.0f});
}

TEST(SceneTest, UpdateSetsLocalMatrixToWorldIfNoParent) {
  liquid::EntityContext context;
  auto local = glm::translate(glm::mat4{1.0f}, {1.0f, 1.0f, 1.0f});
  liquid::SceneNode sceneNode(context.createEntity(), local, nullptr, context);
  sceneNode.update();

  EXPECT_TRUE(
      context.getComponent<liquid::TransformComponent>(sceneNode.getEntity())
          .transformWorld == local);
}

TEST(SceneTest, UpdatesChildrenWithParent) {
  liquid::EntityContext context;
  auto parentTransform = glm::translate(glm::mat4{1.0f}, {1.0f, 1.0f, 1.0f});
  auto childTransform = glm::rotate(glm::mat4{1.0f}, glm::radians(15.0f),
                                    glm::vec3{0.0f, 1.0f, 0.0f});

  liquid::SceneNode parentNode(context.createEntity(), parentTransform, nullptr,
                               context);
  liquid::SharedPtr<liquid::MeshInstance> meshInstance = nullptr;
  auto *childNode = parentNode.addChild(context.createEntity());
  childNode->setTransform(childTransform);

  parentNode.update();
  EXPECT_TRUE(childNode->getWorldTransform() ==
              parentTransform * childTransform);
}

TEST(SceneTest, UpdatesLightIfExists) {
  liquid::EntityContext context;
  auto local = glm::translate(glm::mat4{1.0f}, {1.0f, 0.5f, 1.0f});

  auto entity = context.createEntity();

  liquid::Scene scene(context);

  context.setComponent<liquid::LightComponent>(
      entity, {std::make_shared<liquid::Light>(liquid::Light::DIRECTIONAL,
                                               glm::vec3{}, glm::vec4{}, 1.0)});

  liquid::SceneNode sceneNode(entity, local, nullptr, context);
  sceneNode.update();

  EXPECT_TRUE(context.getComponent<liquid::LightComponent>(entity)
                  .light->getPosition() == glm::vec3(1.0f, 0.5f, 1.0f));
}

TEST(SceneTest, DeletesChildIfExists) {
  liquid::EntityContext context;

  liquid::SceneNode *node = new liquid::SceneNode(
      context.createEntity(), glm::mat4{1.0f}, nullptr, context);

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

  liquid::SceneNode *node = new liquid::SceneNode(
      context.createEntity(), glm::mat4{1.0f}, nullptr, context);

  auto e2 = context.createEntity();
  liquid::SceneNode *node2 =
      new liquid::SceneNode(e2, glm::mat4{1.0f}, nullptr, context);

  node->removeChild(node2);

  EXPECT_TRUE(context.hasComponent<liquid::TransformComponent>(e2));

  delete node;
  delete node2;
}