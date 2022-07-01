#include "liquid/core/Base.h"
#include "liquid/asset/AssetManager.h"
#include "liquid/scripting/ScriptingSystem.h"

#include "liquid-tests/Testing.h"

class EntityQueryLuaInterfaceTest : public ::testing::Test {
public:
  EntityQueryLuaInterfaceTest()
      : assetManager(std::filesystem::current_path()),
        scriptingSystem(eventSystem, assetManager.getRegistry()) {}

  liquid::LuaScope &call(liquid::Entity entity,
                         const liquid::String &functionName) {
    auto handle =
        assetManager
            .loadLuaScriptFromFile(std::filesystem::current_path() /
                                   "scripting-system-component-tester.lua")
            .getData();

    entityDatabase.setComponent<liquid::ScriptingComponent>(entity, {handle});

    scriptingSystem.start(entityDatabase);

    auto &scripting =
        entityDatabase.getComponent<liquid::ScriptingComponent>(entity);

    scripting.scope.luaGetGlobal(functionName);
    scripting.scope.call(0);

    return scripting.scope;
  }

  liquid::EntityDatabase entityDatabase;
  liquid::EventSystem eventSystem;
  liquid::AssetManager assetManager;
  liquid::ScriptingSystem scriptingSystem;
};

TEST_F(EntityQueryLuaInterfaceTest,
       GetEntityByNameReturnsNullIfParametersAreNotPassed) {
  auto entity = entityDatabase.createEntity();
  auto &scope = call(entity, "entity_query_get_first_by_name_no_param");

  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("found_entity"));
}

TEST_F(EntityQueryLuaInterfaceTest,
       GetEntityByNameReturnsNullIfParametersAreInvalid) {
  auto entity = entityDatabase.createEntity();

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
  auto entity = entityDatabase.createEntity();

  auto &scope = call(entity, "entity_query_get_first_by_name");
  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("found_entity"));
}

TEST_F(EntityQueryLuaInterfaceTest,
       GetEntityByNameReturnsEntityTableIfEntityExists) {
  auto entity = entityDatabase.createEntity();

  auto e1 = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::NameComponent>(e1, {"Test"});

  auto &scope = call(entity, "entity_query_get_first_by_name");

  auto table = scope.getGlobal<liquid::LuaTable>("found_entity");
  table.get("id");
  EXPECT_EQ(scope.get<uint32_t>(), e1);
}
