#include "quoll/core/Base.h"
#include "quoll/core/Name.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/entity/EntityQuery.h"
#include "quoll-tests/Testing.h"

class EntityQueryTest : public ::testing::Test {
public:
  EntityQueryTest() : entityQuery(entityDatabase) {}

  quoll::EntityDatabase entityDatabase;
  quoll::EntityQuery entityQuery;
};

TEST_F(EntityQueryTest, ReturnsNullIfEntityWithNameIsNotFound) {
  auto entity = entityDatabase.entity();
  entity.set<quoll::Name>({"test"});

  EXPECT_FALSE(entityQuery.getFirstEntityByName("hello").is_valid());
}

TEST_F(EntityQueryTest, ReturnsEntityIfEntityWithNameIsFound) {
  auto entity = entityDatabase.entity();
  entity.set<quoll::Name>({"test"});

  EXPECT_EQ(entityQuery.getFirstEntityByName("test"), entity);
}

TEST_F(EntityQueryTest,
       ReturnsFirstFoundEntityIfMultipleEntitiesHaveTheSameName) {
  auto entity1 = entityDatabase.entity();
  entity1.set<quoll::Name>({"test"});

  auto entity2 = entityDatabase.entity();
  entity2.set<quoll::Name>({"test"});

  EXPECT_EQ(entityQuery.getFirstEntityByName("test"), entity1);
}
