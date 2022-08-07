#include "liquid/core/Base.h"
#include "liquid/core/EntityDeleter.h"

#include "liquid-tests/Testing.h"

class EntityDeleterTest : public ::testing::Test {
public:
  liquid::EntityDatabase entityDatabase;
  liquid::EntityDeleter entityDeleter;
};

TEST_F(EntityDeleterTest, DeleteEntitiesThatHaveDeleteComponents) {
  static constexpr size_t NumEntities = 20;

  std::vector<liquid::Entity> entities(NumEntities, liquid::EntityNull);
  for (size_t i = 0; i < entities.size(); ++i) {
    auto entity = entityDatabase.createEntity();
    entities.at(i) = entity;

    if ((i % 2) == 0) {
      entityDatabase.setComponent<liquid::DeleteComponent>(entity, {});
    }
  }

  for (auto entity : entities) {
    EXPECT_TRUE(entityDatabase.hasEntity(entity));
  }

  entityDeleter.update(entityDatabase);

  for (size_t i = 0; i < entities.size(); ++i) {
    auto entity = entities.at(i);
    EXPECT_NE(entityDatabase.hasEntity(entity), (i % 2) == 0);
  }
}

TEST_F(EntityDeleterTest, DeletesAllChildrenOfEntitiesWithDeleteComponents) {
  static constexpr size_t NumEntities = 20;

  std::vector<liquid::Entity> entities(NumEntities, liquid::EntityNull);

  for (size_t i = 0; i < entities.size(); ++i) {
    auto entity = entityDatabase.createEntity();
    entities.at(i) = entity;

    if ((i % 2) == 0) {
      entityDatabase.setComponent<liquid::DeleteComponent>(entity, {});
    }

    if (i > 0 && (i % 4) == 0) {
      entityDatabase.setComponent<liquid::ChildrenComponent>(
          entity, {{entities.at(i - 1)}});
    }
  }

  for (auto entity : entities) {
    EXPECT_TRUE(entityDatabase.hasEntity(entity));
  }

  entityDeleter.update(entityDatabase);

  for (size_t i = 0; i < entities.size(); ++i) {
    auto entity = entities.at(i);

    // Even numbers are removed
    bool isEven = (i % 2) == 0;

    // Every value before the fourth item is removed
    bool isChild = (i + 1) < entities.size() && (i + 1) % 4 == 0;
    EXPECT_NE(entityDatabase.hasEntity(entity), isEven || isChild);
  }
}

TEST_F(EntityDeleterTest, RemoveDeletedEntityFromChildrenOfAParent) {
  static constexpr size_t NumEntities = 20;

  std::vector<liquid::Entity> entities(NumEntities, liquid::EntityNull);

  for (size_t i = 0; i < entities.size(); ++i) {
    auto entity = entityDatabase.createEntity();
    entities.at(i) = entity;

    if ((i % 2) == 0) {
      entityDatabase.setComponent<liquid::DeleteComponent>(entity, {});
    }

    if (i > 0) {
      // Set previous entity as parent of this entity
      entityDatabase.setComponent<liquid::ParentComponent>(
          entity, {entities.at(i - 1)});

      // Set this entity as a child of previous entity
      entityDatabase.setComponent<liquid::ChildrenComponent>(entities.at(i - 1),
                                                             {{entity}});
    }
  }

  for (auto entity : entities) {
    EXPECT_TRUE(entityDatabase.hasEntity(entity));
  }

  entityDeleter.update(entityDatabase);

  for (size_t i = 0; i < entities.size(); ++i) {
    if (!entityDatabase.hasComponent<liquid::ChildrenComponent>(
            entities.at(i))) {
      continue;
    }
    auto &children =
        entityDatabase.getComponent<liquid::ChildrenComponent>(entities.at(i))
            .children;

    EXPECT_EQ(children.empty(), (i + 1) % 2 == 0);
  }
}
