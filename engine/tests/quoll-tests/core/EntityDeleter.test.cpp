#include "quoll/core/Base.h"
#include "quoll/core/EntityDeleter.h"

#include "quoll-tests/Testing.h"

class EntityDeleterTest : public ::testing::Test {
public:
  quoll::Scene scene;
  quoll::EntityDeleter entityDeleter;
};

TEST_F(EntityDeleterTest, DeleteEntitiesThatHaveDeleteComponents) {
  static constexpr size_t NumEntities = 20;

  std::vector<quoll::Entity> entities(NumEntities, quoll::Entity::Null);
  for (size_t i = 0; i < entities.size(); ++i) {
    auto entity = scene.entityDatabase.create();
    entities.at(i) = entity;

    if ((i % 2) == 0) {
      scene.entityDatabase.set<quoll::Delete>(entity, {});
    }
  }

  for (auto entity : entities) {
    EXPECT_TRUE(scene.entityDatabase.exists(entity));
  }

  entityDeleter.update(scene);

  for (size_t i = 0; i < entities.size(); ++i) {
    auto entity = entities.at(i);
    EXPECT_NE(scene.entityDatabase.exists(entity), (i % 2) == 0);
  }
}

TEST_F(EntityDeleterTest, DeletesAllChildrenOfEntitiesWithDeleteComponents) {
  static constexpr size_t NumEntities = 20;

  std::vector<quoll::Entity> entities(NumEntities, quoll::Entity::Null);

  for (size_t i = 0; i < entities.size(); ++i) {
    auto entity = scene.entityDatabase.create();
    entities.at(i) = entity;

    if ((i % 2) == 0) {
      scene.entityDatabase.set<quoll::Delete>(entity, {});
    }

    if (i > 0 && (i % 4) == 0) {
      scene.entityDatabase.set<quoll::Children>(entity, {{entities.at(i - 1)}});
    }
  }

  for (auto entity : entities) {
    EXPECT_TRUE(scene.entityDatabase.exists(entity));
  }

  entityDeleter.update(scene);

  for (size_t i = 0; i < entities.size(); ++i) {
    auto entity = entities.at(i);

    // Even numbers are removed
    bool isEven = (i % 2) == 0;

    // Every value before the fourth item is removed
    bool isChild = (i + 1) < entities.size() && (i + 1) % 4 == 0;
    EXPECT_NE(scene.entityDatabase.exists(entity), isEven || isChild);
  }
}

TEST_F(EntityDeleterTest, RemoveDeletedEntityFromChildrenOfAParent) {
  static constexpr size_t NumEntities = 20;

  std::vector<quoll::Entity> entities(NumEntities, quoll::Entity::Null);

  for (size_t i = 0; i < entities.size(); ++i) {
    auto entity = scene.entityDatabase.create();
    entities.at(i) = entity;

    if ((i % 2) == 0) {
      scene.entityDatabase.set<quoll::Delete>(entity, {});
    }

    if (i > 0) {
      // Set previous entity as parent of this entity
      scene.entityDatabase.set<quoll::Parent>(entity, {entities.at(i - 1)});

      // Set this entity as a child of previous entity
      scene.entityDatabase.set<quoll::Children>(entities.at(i - 1), {{entity}});
    }
  }

  for (auto entity : entities) {
    EXPECT_TRUE(scene.entityDatabase.exists(entity));
  }

  entityDeleter.update(scene);

  for (size_t i = 0; i < entities.size(); ++i) {
    if (!scene.entityDatabase.has<quoll::Children>(entities.at(i))) {
      continue;
    }
    auto &children =
        scene.entityDatabase.get<quoll::Children>(entities.at(i)).children;

    EXPECT_EQ(children.empty(), (i + 1) % 2 == 0);
  }
}

TEST_F(EntityDeleterTest, SetsSceneActiveCameraToDummyIfActiveCameraIsDeleted) {
  scene.activeCamera = scene.entityDatabase.create();

  auto entityThatComesAfter = scene.entityDatabase.create();
  scene.entityDatabase.set<quoll::Delete>(scene.activeCamera, {});
  scene.entityDatabase.set<quoll::Delete>(entityThatComesAfter, {});

  entityDeleter.update(scene);

  EXPECT_EQ(scene.activeCamera, scene.dummyCamera);
}
