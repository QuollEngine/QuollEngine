#include "liquid/core/Base.h"
#include "liquid/entity/EntityQuery.h"

#include "liquid-tests/Testing.h"

class EntityQueryTest : public ::testing::Test {
public:
  EntityQueryTest() : entityQuery(entityDatabase) {}

  liquid::EntityDatabase entityDatabase;
  liquid::EntityQuery entityQuery;
};

TEST_F(EntityQueryTest, ReturnsNullEntityIfEntityWithNameIsNotFound) {
  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::Name>(entity, {"test"});

  EXPECT_EQ(entityQuery.getFirstEntityByName("hello"), liquid::Entity::Null);
}

TEST_F(EntityQueryTest, ReturnsEntityIfEntityWithNameIsFound) {
  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::Name>(entity, {"test"});

  EXPECT_EQ(entityQuery.getFirstEntityByName("test"), entity);
}

TEST_F(EntityQueryTest,
       ReturnsFirstFoundEntityIfMultipleEntitiesHaveTheSameName) {
  auto entity1 = entityDatabase.create();
  entityDatabase.set<liquid::Name>(entity1, {"test"});

  auto entity2 = entityDatabase.create();
  entityDatabase.set<liquid::Name>(entity2, {"test"});

  EXPECT_EQ(entityQuery.getFirstEntityByName("test"), entity1);
}
