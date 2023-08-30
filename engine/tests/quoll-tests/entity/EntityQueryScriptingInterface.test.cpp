#include "quoll/core/Base.h"

#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

using EntityQueryLuaInterfaceTest = LuaScriptingInterfaceTestBase;

TEST_F(EntityQueryLuaInterfaceTest,
       GetEntityByNameReturnsNullIfParametersAreNotPassed) {
  auto entity = entityDatabase.create();
  auto &scope = call(entity, "entity_query_get_first_by_name_no_param");

  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("found_entity"));
}

TEST_F(EntityQueryLuaInterfaceTest,
       GetEntityByNameReturnsNullIfParametersAreInvalid) {
  auto entity = entityDatabase.create();

  {
    auto &scope = call(entity, "entity_query_get_first_by_name_param_nil");
    EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("found_entity"));
  }

  {
    auto &scope = call(entity, "entity_query_get_first_by_name_param_boolean");
    EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("found_entity"));
  }

  {
    auto &scope = call(entity, "entity_query_get_first_by_name_param_function");
    EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("found_entity"));
  }

  {
    auto &scope = call(entity, "entity_query_get_first_by_name_param_table");
    EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("found_entity"));
  }
}

TEST_F(EntityQueryLuaInterfaceTest, GetEntityByNameIsNullIfEntityDoesNotExist) {
  auto entity = entityDatabase.create();

  auto &scope = call(entity, "entity_query_get_first_by_name");
  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("found_entity"));
}

TEST_F(EntityQueryLuaInterfaceTest,
       GetEntityByNameReturnsEntityTableIfEntityExists) {
  auto entity = entityDatabase.create();

  auto e1 = entityDatabase.create();
  entityDatabase.set<quoll::Name>(e1, {"Test"});

  auto &scope = call(entity, "entity_query_get_first_by_name");

  auto table = scope.getGlobal<quoll::LuaTable>("found_entity");
  table.get("id");
  EXPECT_EQ(scope.get<quoll::Entity>(), e1);
}

TEST_F(EntityQueryLuaInterfaceTest, DeleteEntityDoesNothingIfInvalidArgument) {
  auto entity = entityDatabase.create();

  {
    call(entity, "entity_query_delete_entity_no_param");
    EXPECT_FALSE(entityDatabase.has<quoll::Delete>(entity));
  }

  {
    call(entity, "entity_query_delete_entity_param_nil");
    EXPECT_FALSE(entityDatabase.has<quoll::Delete>(entity));
  }

  {
    call(entity, "entity_query_delete_entity_param_boolean");
    EXPECT_FALSE(entityDatabase.has<quoll::Delete>(entity));
  }

  {
    call(entity, "entity_query_delete_entity_param_table");
    EXPECT_FALSE(entityDatabase.has<quoll::Delete>(entity));
  }

  {
    call(entity, "entity_query_delete_entity_param_string");
    EXPECT_FALSE(entityDatabase.has<quoll::Delete>(entity));
  }
}

TEST_F(EntityQueryLuaInterfaceTest,
       DeleteEntityDoesNothingIfEntityDoesNotExist) {
  auto entity = entityDatabase.create();

  {
    call(entity, "entity_query_delete_entity_param_invalid_entity");
    EXPECT_FALSE(entityDatabase.has<quoll::Delete>(entity));
  }
}

TEST_F(EntityQueryLuaInterfaceTest,
       DeleteEntityAddsDeleteComponentToExistingEntity) {
  auto entity = entityDatabase.create();

  {
    call(entity, "entity_query_delete_entity_param_entity_table");
    EXPECT_TRUE(entityDatabase.has<quoll::Delete>(entity));
  }
}
