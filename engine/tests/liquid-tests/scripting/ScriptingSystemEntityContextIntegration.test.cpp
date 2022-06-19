#include "liquid/core/Base.h"
#include "liquid/asset/AssetManager.h"
#include "liquid/scripting/ScriptingSystem.h"

#include "liquid-tests/Testing.h"

class ScriptingEntityIntegrationTest : public ::testing::Test {
public:
  ScriptingEntityIntegrationTest()
      : assetManager(std::filesystem::current_path()),
        scriptingSystem(eventSystem, assetManager.getRegistry()) {}

  liquid::LuaScope &call(liquid::Entity entity,
                         const liquid::String &functionName) {
    auto handle =
        assetManager
            .loadLuaScriptFromFile(std::filesystem::current_path() /
                                   "scripting-system-component-tester.lua")
            .getData();

    entityContext.setComponent<liquid::ScriptingComponent>(entity, {handle});

    scriptingSystem.start(entityContext);

    auto &scripting =
        entityContext.getComponent<liquid::ScriptingComponent>(entity);

    scripting.scope.luaGetGlobal(functionName);
    scripting.scope.call(0);

    return scripting.scope;
  }

  liquid::EntityContext entityContext;
  liquid::EventSystem eventSystem;
  liquid::AssetManager assetManager;
  liquid::ScriptingSystem scriptingSystem;
};

using ScriptingNameComponentIntegrationTest = ScriptingEntityIntegrationTest;

TEST_F(ScriptingNameComponentIntegrationTest,
       ReturnsEmptyStringIfNameComponentDoesNotExist) {
  auto entity = entityContext.createEntity();

  auto &scope = call(entity, "name_get");

  EXPECT_FALSE(entityContext.hasComponent<liquid::NameComponent>(entity));
  auto name = scope.getGlobal<liquid::String>("name");
  EXPECT_EQ(name, "");
}

TEST_F(ScriptingNameComponentIntegrationTest,
       ReturnsNameComponentDataIfExists) {
  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::NameComponent>(entity, {"Test name"});

  auto &scope = call(entity, "name_get");

  auto name = scope.getGlobal<liquid::String>("name");
  EXPECT_EQ(name, "Test name");
}

TEST_F(ScriptingNameComponentIntegrationTest,
       DoesNothingIfProvidedArgumentIsNotString) {
  auto entity = entityContext.createEntity();

  EXPECT_FALSE(entityContext.hasComponent<liquid::NameComponent>(entity));
  call(entity, "name_set_invalid");
  EXPECT_FALSE(entityContext.hasComponent<liquid::NameComponent>(entity));
}

TEST_F(ScriptingNameComponentIntegrationTest, CreatesNameComponentOnSet) {
  auto entity = entityContext.createEntity();

  EXPECT_FALSE(entityContext.hasComponent<liquid::NameComponent>(entity));
  call(entity, "name_set");

  EXPECT_EQ(entityContext.getComponent<liquid::NameComponent>(entity).name,
            "Hello World");
}

TEST_F(ScriptingNameComponentIntegrationTest,
       UpdatesExistingNameComponentOnSet) {
  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::NameComponent>(entity, {"Test name"});

  call(entity, "name_set");

  EXPECT_EQ(entityContext.getComponent<liquid::NameComponent>(entity).name,
            "Hello World");
}

using ScriptingLocalTransformComponentTest = ScriptingEntityIntegrationTest;
using ScriptingLocalTransformComponentDeathTest =
    ScriptingEntityIntegrationTest;

TEST_F(ScriptingLocalTransformComponentDeathTest,
       GetPositionFailsIfComponentDoesNotExist) {
  auto entity = entityContext.createEntity();
  EXPECT_DEATH(call(entity, "local_transform_position_get"), ".*");
}

TEST_F(ScriptingLocalTransformComponentTest, GetsPositionValue) {
  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::LocalTransformComponent>(
      entity, {glm::vec3(2.5f, 0.2f, 0.5f)});

  auto &scope = call(entity, "local_transform_position_get");

  EXPECT_EQ(scope.getGlobal<float>("local_position_x"), 2.5f);
  EXPECT_EQ(scope.getGlobal<float>("local_position_y"), 0.2f);
  EXPECT_EQ(scope.getGlobal<float>("local_position_z"), 0.5f);
}

TEST_F(ScriptingLocalTransformComponentDeathTest,
       SetPositionFailsIfComponentDoesNotExist) {
  auto entity = entityContext.createEntity();
  EXPECT_DEATH(call(entity, "local_transform_position_set"), ".*");
}

TEST_F(ScriptingLocalTransformComponentTest, SetsPositionValue) {
  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::LocalTransformComponent>(
      entity, {glm::vec3(1.5f, 0.2f, 0.5f)});

  auto &scope = call(entity, "local_transform_position_set");

  EXPECT_EQ(entityContext.getComponent<liquid::LocalTransformComponent>(entity)
                .localPosition,
            glm::vec3(2.5f, 3.5f, 0.2f));
}

TEST_F(ScriptingLocalTransformComponentTest,
       DoesNothingIfSetPositionArgumentsAreNotNumbers) {
  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::LocalTransformComponent>(
      entity, {{}, {}, glm::vec3(1.5f, 0.2f, 0.5f)});

  auto &scope = call(entity, "local_transform_position_set_invalid");

  EXPECT_EQ(entityContext.getComponent<liquid::LocalTransformComponent>(entity)
                .localScale,
            glm::vec3(1.5f, 0.2f, 0.5f));
}

TEST_F(ScriptingLocalTransformComponentDeathTest,
       GetScaleFailsIfComponentDoesNotExist) {
  auto entity = entityContext.createEntity();
  EXPECT_DEATH(call(entity, "local_transform_scale_get"), ".*");
}

TEST_F(ScriptingLocalTransformComponentTest, GetsScaleValue) {
  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::LocalTransformComponent>(
      entity, {{}, {}, glm::vec3(2.5f, 0.2f, 0.5f)});

  auto &scope = call(entity, "local_transform_scale_get");

  EXPECT_EQ(scope.getGlobal<float>("local_scale_x"), 2.5f);
  EXPECT_EQ(scope.getGlobal<float>("local_scale_y"), 0.2f);
  EXPECT_EQ(scope.getGlobal<float>("local_scale_z"), 0.5f);
}

TEST_F(ScriptingLocalTransformComponentDeathTest,
       SetScaleFailsIfComponentDoesNotExist) {
  auto entity = entityContext.createEntity();
  EXPECT_DEATH(call(entity, "local_transform_scale_set"), ".*");
}

TEST_F(ScriptingLocalTransformComponentTest, SetsScaleValue) {
  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::LocalTransformComponent>(
      entity, {{}, {}, glm::vec3(1.5f, 0.2f, 0.5f)});

  auto &scope = call(entity, "local_transform_scale_set");

  EXPECT_EQ(entityContext.getComponent<liquid::LocalTransformComponent>(entity)
                .localScale,
            glm::vec3(2.5f, 3.5f, 0.2f));
}

TEST_F(ScriptingLocalTransformComponentTest,
       DoesNothingIfSetScaleArgumentsAreNotNumbers) {
  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::LocalTransformComponent>(
      entity, {{}, {}, glm::vec3(1.5f, 0.2f, 0.5f)});

  auto &scope = call(entity, "local_transform_scale_set_invalid");

  EXPECT_EQ(entityContext.getComponent<liquid::LocalTransformComponent>(entity)
                .localScale,
            glm::vec3(1.5f, 0.2f, 0.5f));
}
