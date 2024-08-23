#include "quoll/core/Base.h"
#include "quoll/core/Delete.h"
#include "quoll/core/EntityDeleter.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/scene/Children.h"
#include "quoll/scene/Parent.h"
#include "quoll/scene/Scene.h"
#include "quoll/system/SystemView.h"
#include "quoll-tests/Testing.h"

class EntityDeleterTest : public ::testing::Test {
public:
  quoll::Scene scene;
  quoll::EntityDeleter entityDeleter;
  quoll::SystemView view{&scene};
};

TEST_F(EntityDeleterTest, DeleteEntitiesThatHaveDeleteComponents) {
  static constexpr usize NumEntities = 20;

  std::vector<quoll::Entity> entities(NumEntities);
  for (usize i = 0; i < entities.size(); ++i) {
    auto entity = scene.entityDatabase.entity();
    entities.at(i) = entity;

    if ((i % 2) == 0) {
      entity.add<quoll::Delete>();
    }
  }

  for (auto entity : entities) {
    EXPECT_TRUE(entity.is_valid());
  }

  entityDeleter.update(view);

  for (usize i = 0; i < entities.size(); ++i) {
    auto entity = entities.at(i);
    EXPECT_NE(entity.is_valid(), (i % 2) == 0);
  }
}

TEST_F(EntityDeleterTest, DeletesAllChildrenOfEntitiesWithDeleteComponents) {
  static constexpr usize NumEntities = 20;

  std::vector<quoll::Entity> entities(NumEntities);

  for (usize i = 0; i < entities.size(); ++i) {
    auto entity = scene.entityDatabase.entity();
    entities.at(i) = entity;

    if ((i % 2) == 0) {
      entity.add<quoll::Delete>();
    }

    if (i > 0 && (i % 4) == 0) {
      entity.set<quoll::Children>({{entities.at(i - 1)}});
    }
  }

  for (auto entity : entities) {
    EXPECT_TRUE(entity.is_valid());
  }

  entityDeleter.update(view);

  for (usize i = 0; i < entities.size(); ++i) {
    auto entity = entities.at(i);

    // Even numbers are removed
    bool isEven = (i % 2) == 0;

    // Every value before the fourth item is removed
    bool isChild = (i + 1) < entities.size() && (i + 1) % 4 == 0;
    EXPECT_NE(entity.is_valid(), isEven || isChild);
  }
}

TEST_F(EntityDeleterTest, RemoveDeletedEntityFromChildrenOfAParent) {
  static constexpr usize NumEntities = 20;

  std::vector<quoll::Entity> entities(NumEntities);

  for (usize i = 0; i < entities.size(); ++i) {
    auto entity = scene.entityDatabase.entity();
    entities.at(i) = entity;

    if ((i % 2) == 0) {
      entity.add<quoll::Delete>();
    }

    if (i > 0) {
      // Set previous entity as parent of this entity
      entity.set<quoll::Parent>({entities.at(i - 1)});

      // Set this entity as a child of previous entity
      entities.at(i - 1).set<quoll::Children>({{entity}});
    }
  }

  for (auto entity : entities) {
    EXPECT_TRUE(entity.is_valid());
  }

  entityDeleter.update(view);

  for (usize i = 0; i < entities.size(); ++i) {
    auto entity = entities.at(i);
    if (!entity.is_valid() || entity.has<quoll::Children>()) {
      continue;
    }

    auto &children = entity.get_ref<quoll::Children>()->children;
    EXPECT_EQ(children.empty(), (i + 1) % 2 == 0);
  }
}

TEST_F(EntityDeleterTest, SetsSceneActiveCameraToDummyIfActiveCameraIsDeleted) {
  scene.activeCamera = scene.entityDatabase.entity();

  auto entityThatComesAfter = scene.entityDatabase.entity();
  scene.activeCamera.add<quoll::Delete>();
  entityThatComesAfter.add<quoll::Delete>();

  entityDeleter.update(view);

  EXPECT_EQ(scene.activeCamera, scene.dummyCamera);
}
