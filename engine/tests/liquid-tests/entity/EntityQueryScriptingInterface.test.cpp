#include "liquid/core/Base.h"
#include "liquid/asset/AssetCache.h"
#include "liquid/scripting/ScriptingSystem.h"

#include "liquid-tests/Testing.h"

class EntityQueryLuaInterfaceTest : public ::testing::Test {
public:
  EntityQueryLuaInterfaceTest()
      : assetCache(std::filesystem::current_path()),
        scriptingSystem(eventSystem, assetCache.getRegistry()) {}

  liquid::LuaScope &call(liquid::Entity entity,
                         const liquid::String &functionName) {
    auto handle =
        assetCache
            .loadLuaScriptFromFile(std::filesystem::current_path() /
                                   "scripting-system-component-tester.lua")
            .getData();

    entityDatabase.set<liquid::Script>(entity, {handle});

    scriptingSystem.start(entityDatabase);

    auto &scripting = entityDatabase.get<liquid::Script>(entity);

    scripting.scope.luaGetGlobal(functionName);
    scripting.scope.call(0);

    return scripting.scope;
  }

  liquid::EntityDatabase entityDatabase;
  liquid::EventSystem eventSystem;
  liquid::AssetCache assetCache;
  liquid::ScriptingSystem scriptingSystem;
};

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
  entityDatabase.set<liquid::Name>(e1, {"Test"});

  auto &scope = call(entity, "entity_query_get_first_by_name");

  auto table = scope.getGlobal<liquid::LuaTable>("found_entity");
  table.get("id");
  EXPECT_EQ(scope.get<liquid::Entity>(), e1);
}
