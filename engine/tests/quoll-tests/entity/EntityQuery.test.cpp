#include "quoll/core/Base.h"
#include "quoll/entity/EntityQuery.h"

#include "quoll-tests/Testing.h"

class EntityQueryTest : public ::testing::Test {
public:
  EntityQueryTest() : entityQuery(entityDatabase) {}

  quoll::EntityDatabase entityDatabase;
  quoll::EntityQuery entityQuery;
};

TEST_F(EntityQueryTest, ReturnsNullEntityIfEntityWithNameIsNotFound) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Name>(entity, {"test"});

  EXPECT_EQ(entityQuery.getFirstEntityByName("hello"), quoll::Entity::Null);
}

TEST_F(EntityQueryTest, ReturnsEntityIfEntityWithNameIsFound) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Name>(entity, {"test"});

  EXPECT_EQ(entityQuery.getFirstEntityByName("test"), entity);
}

TEST_F(EntityQueryTest,
       ReturnsFirstFoundEntityIfMultipleEntitiesHaveTheSameName) {
  auto entity1 = entityDatabase.create();
  entityDatabase.set<quoll::Name>(entity1, {"test"});

  auto entity2 = entityDatabase.create();
  entityDatabase.set<quoll::Name>(entity2, {"test"});

  EXPECT_EQ(entityQuery.getFirstEntityByName("test"), entity1);
}
