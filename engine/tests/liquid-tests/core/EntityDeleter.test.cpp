#include "liquid/core/Base.h"
#include "liquid/core/EntityDeleter.h"

#include "liquid-tests/Testing.h"

class EntityDeleterTest : public ::testing::Test {
public:
  liquid::Scene scene;
  liquid::EntityDeleter entityDeleter;
};

TEST_F(EntityDeleterTest, DeleteEntitiesThatHaveDeleteComponents) {
  static constexpr size_t NumEntities = 20;

  std::vector<liquid::Entity> entities(NumEntities, liquid::EntityNull);
  for (size_t i = 0; i < entities.size(); ++i) {
    auto entity = scene.entityDatabase.create();
    entities.at(i) = entity;

    if ((i % 2) == 0) {
      scene.entityDatabase.set<liquid::Delete>(entity, {});
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

  std::vector<liquid::Entity> entities(NumEntities, liquid::EntityNull);

  for (size_t i = 0; i < entities.size(); ++i) {
    auto entity = scene.entityDatabase.create();
    entities.at(i) = entity;

    if ((i % 2) == 0) {
      scene.entityDatabase.set<liquid::Delete>(entity, {});
    }

    if (i > 0 && (i % 4) == 0) {
      scene.entityDatabase.set<liquid::Children>(entity,
                                                 {{entities.at(i - 1)}});
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

  std::vector<liquid::Entity> entities(NumEntities, liquid::EntityNull);

  for (size_t i = 0; i < entities.size(); ++i) {
    auto entity = scene.entityDatabase.create();
    entities.at(i) = entity;

    if ((i % 2) == 0) {
      scene.entityDatabase.set<liquid::Delete>(entity, {});
    }

    if (i > 0) {
      // Set previous entity as parent of this entity
      scene.entityDatabase.set<liquid::Parent>(entity, {entities.at(i - 1)});

      // Set this entity as a child of previous entity
      scene.entityDatabase.set<liquid::Children>(entities.at(i - 1),
                                                 {{entity}});
    }
  }

  for (auto entity : entities) {
    EXPECT_TRUE(scene.entityDatabase.exists(entity));
  }

  entityDeleter.update(scene);

  for (size_t i = 0; i < entities.size(); ++i) {
    if (!scene.entityDatabase.has<liquid::Children>(entities.at(i))) {
      continue;
    }
    auto &children =
        scene.entityDatabase.get<liquid::Children>(entities.at(i)).children;

    EXPECT_EQ(children.empty(), (i + 1) % 2 == 0);
  }
}

TEST_F(EntityDeleterTest, SetsSceneActiveCameraToDummyIfActiveCameraIsDeleted) {
  scene.activeCamera = scene.entityDatabase.create();

  auto entityThatComesAfter = scene.entityDatabase.create();
  scene.entityDatabase.set<liquid::Delete>(scene.activeCamera, {});
  scene.entityDatabase.set<liquid::Delete>(entityThatComesAfter, {});

  entityDeleter.update(scene);

  EXPECT_EQ(scene.activeCamera, scene.dummyCamera);
}
